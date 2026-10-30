#[derive(Debug)]
pub struct Pixel24 {
    _red: u8,
    _green: u8,
    _blue: u8
}

#[derive(Debug)]
pub struct Image {
    _pixels: Vec<Vec<Pixel24>>
}

pub struct BitmapInfoHeader {
    #[allow(unused)]
    hdr_size: u32,
    #[allow(unused)]
	width: u32,
    #[allow(unused)]
	height: u32,
    #[allow(unused)]
	planes: u16,
    #[allow(unused)]
	bits_per_pixel: u16,
    #[allow(unused)]
	compression: u32,
    #[allow(unused)]
	image_size: u32,
    #[allow(unused)]
	x_pixels_per_m: u32,
    #[allow(unused)]
	y_pixels_per_m: u32,
    #[allow(unused)]
	colors_used: u32,
    #[allow(unused)]
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