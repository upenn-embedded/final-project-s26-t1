use std::sync::Arc;

use eframe::{egui::{self, Color32, ColorImage, Pos2}, egui_wgpu::{WgpuConfiguration, WgpuSetup, WgpuSetupCreateNew}, wgpu};

fn main() -> eframe::Result {
    // force limits lower for raspberry pi
    let mut wgpu_setup_create_new = WgpuSetupCreateNew::without_display_handle();
    wgpu_setup_create_new.device_descriptor = Arc::new(|_adapter| {
        let base_limits = wgpu::Limits::downlevel_webgl2_defaults();

        wgpu::DeviceDescriptor {
            label: Some("egui wgpu device"),
            required_limits: wgpu::Limits {
                // When using a depth buffer, we have to be able to create a texture
                // large enough for the entire surface, and we want to support 4k+ displays.
                max_texture_dimension_2d: 8192,
                ..base_limits
            },
            ..Default::default()
        }
    });

    let native_options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_fullscreen(true),
        wgpu_options: WgpuConfiguration {
            wgpu_setup: WgpuSetup::CreateNew(wgpu_setup_create_new),
            ..Default::default()
        },
        ..Default::default()
    };
    eframe::run_native(
        "Etch",
        native_options,
        Box::new(|cc| Ok(Box::new(App::new(cc))))
    )
}

struct App {
    frame_counter: usize,
    image: Option<ColorImage>,
    texture: Option<egui::TextureHandle>,
    last_cursor_pos: Option<Pos2>,
    blank: bool,
}

impl App {
    fn new(_cc: &eframe::CreationContext) -> Self {
        Self {
            image: None,
            frame_counter: 0,
            texture: None,
            last_cursor_pos: None,
            blank: true,
        }
    }
}

impl eframe::App for App {
    fn ui(&mut self, ui: &mut egui::Ui, _frame: &mut eframe::Frame) {
        if self.texture.is_none() {
            // TODO: find a less dumb way to do this
            if self.frame_counter < 4 { // wait until monitor size propagates
                self.frame_counter += 1;
                // make sure monitor scaling factor is ignored
                ui.ctx().set_pixels_per_point(1.0);
                return;
            } else {
                let size = ui.ctx().viewport_rect().size();
                let scale = ui.ctx().pixels_per_point();

                assert!(scale == 1.0, "Scale should be 1");
                println!("got monitor size of {}", size);

                self.image = Some(ColorImage::filled([size.x as usize, size.y as usize], Color32::WHITE));
            }
        }

        let image = self.image.as_mut().expect("image should be here");

        let texture_handle = self.texture.get_or_insert_with(|| {
            ui.ctx().load_texture(
                "my-image",
                image.clone(),
                Default::default()
            )
        });

        if let Some(cursor_pos) = ui.input(|i| i.pointer.hover_pos()) {
            // discard if not in screen
            if
                cursor_pos.x >= 0.0
                && cursor_pos.x <= image.width() as f32
                && cursor_pos.y >= 0.0
                && cursor_pos.y <= image.height() as f32
            {
                if
                    let Some(last_cursor_pos) = self.last_cursor_pos.as_mut()
                    // prevent jumps on start
                    && !(
                        self.blank
                        && (
                            (last_cursor_pos.x-cursor_pos.x).abs() > image.width() as f32/10.0
                            || (last_cursor_pos.y-cursor_pos.y).abs() > image.height() as f32/10.0
                        )
                    )
                {
                    let buffer = 4;
                    let rect_x1: usize = usize::saturating_sub(
                        last_cursor_pos.x.min(cursor_pos.x) as usize,
                        buffer
                    );
                    let rect_x2: usize = usize::min(
                        image.width()-1,
                        last_cursor_pos.x.max(cursor_pos.x) as usize + buffer,
                    );
                    let rect_y1: usize = usize::saturating_sub(
                        last_cursor_pos.y.min(cursor_pos.y) as usize,
                        buffer
                    );
                    let rect_y2: usize = usize::min(
                        image.height()-1,
                        last_cursor_pos.y.max(cursor_pos.y) as usize + buffer,
                    );

                    for x in rect_x1..=rect_x2 {
                        let xf = x as f32;
                        for y in rect_y1..=rect_y2 {
                            let yf = y as f32;
                            let dist_from_line =
                                (
                                    (cursor_pos.y-last_cursor_pos.y)*xf
                                    -(cursor_pos.x-last_cursor_pos.x)*yf
                                    +cursor_pos.x*last_cursor_pos.y
                                    -cursor_pos.y*last_cursor_pos.x
                                ).abs()
                                /(
                                    (cursor_pos.y-last_cursor_pos.y)*(cursor_pos.y-last_cursor_pos.y)
                                    +(cursor_pos.x-last_cursor_pos.x)*(cursor_pos.x-last_cursor_pos.x)
                                ).sqrt();
                            if dist_from_line < 1.0 {
                                let index = image.width()*y+x;
                                image.pixels[index] = Color32::BLACK;
                            }
                        }
                    }

                    *last_cursor_pos = cursor_pos;
                    self.blank = false;
                    texture_handle.set(image.clone(), Default::default());
                } else {
                    self.last_cursor_pos = Some(cursor_pos);
                }
            }
        }

        ui.image(egui::load::SizedTexture::from_handle(texture_handle));
    }
}
