use eframe::egui::{self, Color32, ColorImage};

fn main() -> eframe::Result {
    let native_options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_fullscreen(true),
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
}

impl App {
    fn new(cc: &eframe::CreationContext) -> Self {
        Self {
            image: None,
            frame_counter: 0,
            texture: None,
        }
    }
}

impl eframe::App for App {
    fn ui(&mut self, ui: &mut egui::Ui, frame: &mut eframe::Frame) {
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

        let texture_handle = self.texture.get_or_insert_with(|| {
            ui.ctx().load_texture(
                "my-image",
                self.image.clone().expect("image should be here"),
                Default::default()
            )
        });

        ui.image(egui::load::SizedTexture::from_handle(texture_handle));
    }
}
