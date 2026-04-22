use std::{fs::{self, File, OpenOptions}, io::{BufReader, BufWriter, Read, Seek, SeekFrom, Write}, ptr::read, sync::Arc};

use bytemuck::{cast_slice, try_cast_vec};
use eframe::{egui::{self, Color32, ColorImage, Key, Pos2}, egui_wgpu::{WgpuConfiguration, WgpuSetup, WgpuSetupCreateNew}, wgpu::{self, Features}};
use serde::{Deserialize, Serialize};
use serde_json::to_string;

fn main() -> eframe::Result {
    // force limits lower for raspberry pi
    let mut wgpu_setup_create_new = WgpuSetupCreateNew::without_display_handle();
    wgpu_setup_create_new.device_descriptor = Arc::new(|_adapter| {
        let base_limits = wgpu::Limits::downlevel_webgl2_defaults();

        wgpu::DeviceDescriptor {
            label: Some("egui wgpu device"),
            required_features: Features::empty(),
            required_limits: wgpu::Limits {
                max_texture_dimension_2d: 3840, // slightly larger for laptop display
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
    file_set: Option<FileSet>,
}

impl App {
    fn new(_cc: &eframe::CreationContext) -> Self {
        Self {
            image: None,
            frame_counter: 0,
            texture: None,
            last_cursor_pos: None,
            blank: true,
            file_set: None,
        }
    }
}

impl eframe::App for App {
    fn ui(&mut self, ui: &mut egui::Ui, _frame: &mut eframe::Frame) {
        if self.texture.is_none() {
            // TODO: find a less dumb way to do this
            if self.frame_counter < 3 { // wait until monitor size propagates
                self.frame_counter += 1;
                // make sure monitor scaling factor is ignored
                ui.ctx().set_pixels_per_point(1.0);
                return;
            } else {
                let size = ui.ctx().viewport_rect().size();
                let scale = ui.ctx().pixels_per_point();

                assert!(scale == 1.0, "Scale should be 1");
                println!("got monitor size of {}", size);

                if let Some(mut file_set) = FileSet::get(size.x as usize, size.y as usize) {
                    self.image = Some(ColorImage::new(
                        [
                            size.x as usize,
                            size.y as usize,
                        ],
                        file_set.pixels(),
                    ));

                    self.file_set = Some(file_set);
                } else {
                    self.image = Some(ColorImage::filled([size.x as usize, size.y as usize], Color32::WHITE));

                    FileSet::new(self.image.as_ref().unwrap(), 0.0, 0.0);
                }
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
                    && let dx = cursor_pos.x-last_cursor_pos.x
                    && let dy = cursor_pos.y-last_cursor_pos.y
                    && let dxy2 = dx*dx+dy*dy
                    // prevent jumps on start
                    && !(self.blank && dxy2 > 10.0)
                {
                    let buffer = 1;
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

        if ui.input(|i| i.modifiers.ctrl && i.key_pressed(Key::Z)) {
            if let Some(cursor_pos) = self.last_cursor_pos {
                if
                    let Some(file_set) = self.file_set.as_mut()
                {
                    file_set.save(image, cursor_pos.x, cursor_pos.y);
                }
                
                *image = ColorImage::filled([image.width(), image.height()], Color32::WHITE);
                texture_handle.set(image.clone(), Default::default());

                self.file_set = Some(FileSet::new(image, cursor_pos.x, cursor_pos.y));
            }
        }

        ui.image(egui::load::SizedTexture::from_handle(texture_handle));
    }

    fn on_exit(&mut self) {
        if
            let Some(mut file_set) = self.file_set.take()
            && let Some(image) = self.image.as_ref()
            && let Some(cursor_pos) = self.last_cursor_pos
        {
            file_set.save(image, cursor_pos.x, cursor_pos.y);
            drop(file_set);
        }
    }
}

#[derive(Serialize, Deserialize)]
struct Metadata {
    version: usize,
    cursor_x: f32,
    cursor_y: f32,
}

impl Metadata {
    fn new(cursor_x: f32, cursor_y: f32) -> Self {
        Self {
            version: 1,
            cursor_x,
            cursor_y,
        }
    }

    fn to_json(&self) -> String {
        serde_json::to_string(self).expect("Failed to convert metadata to string")
    }
}

struct FileSet {
    image: File,
    metadata: File,
}

impl FileSet {
    fn new(initial_image: &ColorImage, cursor_x: f32, cursor_y: f32) -> FileSet {
        let mut path = dirs::picture_dir().expect("Must have a pictures directory");
        assert!(path.is_dir(), "Pictures dir must exist");

        path.push("Etches");
        if !path.is_dir() {
            fs::create_dir(&path).expect("Failed to make Etches directory");
        }

        path.push(format!("{}x{}", initial_image.width(), initial_image.height()));
        if !path.is_dir() {
            fs::create_dir(&path).expect("Failed to make directory for resolution");
        }

        let filename = chrono::Local::now().format("%+").to_string();
        path.push(&filename);

        let mut metadata_path = path.clone();
        metadata_path.add_extension("json");

        let image_path = { path.add_extension("png"); path };

        assert!(!metadata_path.exists(), "What to do if metadata file already exists not done");
        assert!(!image_path.exists(), "What to do if image file already exists not done");

        let image = fs::File::create_new(image_path).expect("Failed to create image file");

        let metadata = fs::File::create_new(metadata_path).expect("Failed to create metadata file");
        
        let mut file_set = FileSet {
            image,
            metadata,
        };

        file_set.save(initial_image, cursor_x, cursor_y);

        file_set
    }

    fn get(width: usize, height: usize) -> Option<FileSet> {
        let Some(mut path) = dirs::picture_dir() else { return None };

        path.extend(["Etches", format!("{}x{}", width, height).as_str()]);

        let Ok(read_dir) = path.read_dir() else { return None };

        let mut filename = String::new();
        let mut image: Option<File> = None;
        let mut metadata: Option<File> = None;

        for file in read_dir {
            let Ok(dirent) = file else { continue };

            let Some(Some(stem)) = dirent.path().file_stem().map(|s| s.to_str().map(|s| s.to_string())) else { continue };

            if stem > filename || filename.is_empty() {
                filename.clear();
                filename.push_str(&stem);
                image = None;
                metadata = None;
            }

            if stem == filename {
                match dirent.path().extension().map(|s| s.to_str()) {
                    Some(Some("png")) => if let Ok(opened_image) = OpenOptions::new().read(true).write(true).open(dirent.path()) {
                        image = Some(opened_image);
                    },
                    Some(Some("json")) => if let Ok(opened_metadata) = OpenOptions::new().read(true).write(true).open(dirent.path()) {
                        metadata = Some(opened_metadata);
                    },
                    _ => {}
                }
            }
        }

        if let Some(image) = image && let Some(metadata) = metadata {
            Some(FileSet {
                image,
                metadata,
            })
        } else {
            None
        }
    }

    fn save(&mut self, image: &ColorImage, cursor_x: f32, cursor_y: f32) {
        self.image.set_len(0).expect("Failed to truncate image");
        self.image.seek(SeekFrom::Start(0)).expect("Failed to sek in image");

        let mut encoder = png::Encoder::new(BufWriter::new(&mut self.image), image.width() as u32, image.height() as u32);
        encoder.set_color(png::ColorType::Rgba);
        encoder.set_depth(png::BitDepth::Eight);
        let mut encoder = encoder.write_header().unwrap();
        encoder.write_image_data(cast_slice(image.pixels.as_slice())).unwrap();
        encoder.finish().unwrap();

        self.metadata.write_all(Metadata::new(cursor_x, cursor_y).to_json().as_bytes()).expect("Failed to write metadata data");
    }

    fn pixels(&mut self) -> Vec<Color32> {
        //self.image.read_to_end(&mut pixels).expect("Failed to read image");
        let decoder = png::Decoder::new(BufReader::new(&mut self.image));
        let mut decoder = decoder.read_info().unwrap();
        let mut pixels = vec![0; decoder.output_buffer_size().unwrap()]; // slower to make new vec, but don't want to do
                                     // crazy casts rn
        decoder.next_frame(&mut pixels).unwrap();

        let pixels = Vec::from(cast_slice(pixels.as_slice()));

        pixels
    }
}
