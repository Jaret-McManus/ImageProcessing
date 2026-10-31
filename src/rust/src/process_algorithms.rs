use crate::data_structures::{Image, Pixel24};

enum _OutOfBoundsMethod {
    Extend,
    // Blackout,
    // Mirror,
}

pub fn box_blur(image: Image, kernel_size: u32) -> Image {
    let mut new_image = Image::empty(image.width, image.height);
    
    for y in 0..image.height {
        let mut new_row: Vec<Pixel24> = Vec::with_capacity(image.width as usize);
        for x in 0..image.width {
            let new_pixel = get_blurred_pixel(x, y, kernel_size, &image);

            new_row.push(new_pixel);
        }

        new_image.push_row(new_row);
    }

    new_image
}


pub fn identity(image: Image) -> Image {
    image
}


fn get_blurred_pixel(x: usize, y: usize, _kernel_size: u32, image: &Image) -> Pixel24 {
    let original_pixel = &image.pixels[x][y];
    return Pixel24 { 
        red: original_pixel.red,
        green:  original_pixel.green,
        blue: original_pixel.blue
    };
}