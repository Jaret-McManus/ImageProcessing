use std::collections::VecDeque;
use crate::file_io::FileIOError;

#[derive(Debug, Clone)]
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
    pub pixels: VecDeque<Vec<Pixel24>>,
    pub width: usize,
    pub height: usize,
}

impl Image {
    pub fn empty(width: usize, height: usize) -> Self {
        let rows = VecDeque::<Vec<Pixel24>>::with_capacity(height as usize);

        Image { 
            pixels: rows, 
            width, height
        }
    }

    pub fn push_row_backwards(&mut self, row: Vec<Pixel24>) -> () {
        self.pixels.push_front(row);
    }

    pub fn push_row(&mut self, row: Vec<Pixel24>) -> () {
        self.pixels.push_back(row);
    }
}

#[derive(Debug)]
pub struct BitmapHeader {
	pub size: u32,
	pub reserved1: u16,
	pub reserved2: u16,
	pub offset: u32,
}

impl BitmapHeader {
    pub fn try_create(
        size: Result<u32, FileIOError>,
        reserved1: Result<u16, FileIOError>,
        reserved2: Result<u16, FileIOError>,
        offset: Result<u32, FileIOError>,
    ) -> Result<BitmapHeader, FileIOError> {
        Ok(BitmapHeader {
            size: size?,
            reserved1: reserved1?,
            reserved2: reserved2?,
            offset: offset?
        })
    }
}

#[derive(Debug)]
pub struct BitmapInfoHeader {
    pub hdr_size: u32,
	pub width: u32,
	pub height: u32,
	pub planes: u16,
	pub bits_per_pixel: u16,
	pub compression: u32,
	pub image_size: u32,
	pub x_pixels_per_m: u32,
	pub y_pixels_per_m: u32,
	pub colors_used: u32,
	pub important_colors: u32,
}

impl BitmapInfoHeader {
    pub fn try_create(
        hdr_size: Result<u32, FileIOError>,
        width: Result<u32, FileIOError>,
        height: Result<u32, FileIOError>,
        planes: Result<u16, FileIOError>,
        bits_per_pixel: Result<u16, FileIOError>,
        compression: Result<u32, FileIOError>,
        image_size: Result<u32, FileIOError>,
        x_pixels_per_m: Result<u32, FileIOError>,
        y_pixels_per_m: Result<u32, FileIOError>,
        colors_used: Result<u32, FileIOError>,
        important_colors: Result<u32, FileIOError>,
    ) -> Result<BitmapInfoHeader, FileIOError> {
        Ok(BitmapInfoHeader{
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