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