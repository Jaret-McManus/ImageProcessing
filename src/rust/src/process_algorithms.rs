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

        if y % 10 == 0 {
            eprint!(    
                "\rwritten {:5}/{} rows: {:5.2}% done", 
                y, 
                image.height, 
                y as f64 / image.height as f64 * 100.0
            );
        }
        new_image.push_row(new_row);
    }

    new_image
}


pub fn identity(image: Image) -> Image {
    image
}


fn get_blurred_pixel(x: usize, y: usize, kernel_size: u32, image: &Image) -> Pixel24 {
    let kernel_size = kernel_size as i64;
    let (x, y) = (x as i64, y as i64);
    let mut red = 0.0;
    let mut green = 0.0;
    let mut blue = 0.0;
    for i in -kernel_size..kernel_size {
        for j in -kernel_size..kernel_size {
            let pixel = get_pixel_extend(x + i, y + j, image);
            red += pixel.red as f64;
            green += pixel.green as f64;
            blue += pixel.blue as f64;
        }
    }

    let kernel_length = kernel_size as f64 * 2.0 + 1.0;
    let num_pixels = kernel_length * kernel_length;
    red /= num_pixels;
    blue /= num_pixels;
    green /= num_pixels;

    Pixel24 { red: red as u8, green: green as u8, blue: blue as u8 }
}

fn get_pixel_extend(x: i64, y: i64, image: &Image) -> Pixel24 {
    let x = x.clamp(0i64, image.width as i64 - 1) 
        as usize;
    
    let y = y.clamp(0i64, image.height as i64 - 1) 
        as usize;


    image.pixels[y as usize][x as usize].clone()
}