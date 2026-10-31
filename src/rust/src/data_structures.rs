#[derive(Debug)]
pub struct Pixel24 {
    pub red: u8,
    pub green: u8,
    pub blue: u8
}

impl Pixel24 {
    pub fn new(red: u8, green: u8, blue: u8) -> Self {
        Pixel24 { red, green, blue }
    }
}

#[derive(Debug)]
pub struct Image {
    pub pixels: Vec<Vec<Pixel24>>,
    last_populated_row: i64,
    pub width: usize,
    pub height: usize,
}

impl Image {
    pub fn empty(width: usize, height: usize) -> Self {
        let mut rows = Vec::<Vec<Pixel24>>::with_capacity(height as usize);
        for _ in 0..height {
            rows.push(Vec::<Pixel24>::with_capacity(width as usize));
        }

        Image { 
            pixels: rows, 
            last_populated_row: (height - 1) as i64,
            width, height
        }
    }

    pub fn push_row(&mut self, row: Vec<Pixel24>) -> () {
        assert!(self.last_populated_row >= 0);

        self.pixels.insert(self.last_populated_row as usize, row);
        self.last_populated_row -= 1;
    }
}

pub struct BitmapHeader {
	size: u32,
	reserved1: u16,
	reserved2: u16,
	pub offset: u32,
}

impl BitmapHeader {
    pub fn try_create(
        size: Option<u32>,
        reserved1: Option<u16>,
        reserved2: Option<u16>,
        offset: Option<u32>,
    ) -> Option<BitmapHeader> {
        Some(BitmapHeader {
            size: size?,
            reserved1: reserved1?,
            reserved2: reserved2?,
            offset: offset?
        })
    }
}

pub struct BitmapInfoHeader {
    hdr_size: u32,
	pub width: u32,
	pub height: u32,
	planes: u16,
	bits_per_pixel: u16,
	compression: u32,
	image_size: u32,
	x_pixels_per_m: u32,
	y_pixels_per_m: u32,
	colors_used: u32,
	important_colors: u32,
}

impl BitmapInfoHeader {
    pub fn try_create(
        hdr_size: Option<u32>,
        width: Option<u32>,
        height: Option<u32>,
        planes: Option<u16>,
        bits_per_pixel: Option<u16>,
        compression: Option<u32>,
        image_size: Option<u32>,
        x_pixels_per_m: Option<u32>,
        y_pixels_per_m: Option<u32>,
        colors_used: Option<u32>,
        important_colors: Option<u32>,
    ) -> Option<BitmapInfoHeader> {
        Some(BitmapInfoHeader{
            hdr_size: hdr_size?,
            width: width?,
            height: height?,
            planes: planes?,
            bits_per_pixel: bits_per_pixel?,
            compression: compression?,
            image_size: image_size?,
            x_pixels_per_m: x_pixels_per_m?,
            y_pixels_per_m: y_pixels_per_m?,
            colors_used: colors_used?,
            important_colors: important_colors?,
        })
    }
}