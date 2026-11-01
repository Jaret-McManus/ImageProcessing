use std::{
    collections::VecDeque, env, process::exit, time::Instant
};
mod arg_collector;
mod file_io;
mod data_structures;
mod process_algorithms;
use crate::{
    arg_collector::{ArgCollector, ProcessType},
    data_structures::{Image}
};

fn main() {
    let now = Instant::now();
    let args: VecDeque<String> = env::args().skip(1).collect();

    let collector_result = ArgCollector::create(args);
    let Ok(collector) = collector_result else {
        println!("Error collecting arguments: {:?}", collector_result.unwrap_err());
        exit(1);
    };

    let image_result = file_io::read_file_to_image(collector.input_filename);
    let Ok((
        image,
        bm_header,
        bm_info_header
    )) = image_result else {
        println!("Error reading image: {:?}", image_result.unwrap_err());
        exit(-1);
    };

    if collector.debug_flag {
        println!("Finished reading image in {:?}", now.elapsed());
    }

    if collector.print_headers_flag { dbg!(&bm_header); dbg!(&bm_info_header); }
    
    let new_image: Image;
    match collector.process_type {
        ProcessType::BoxBlur(kernel_size) => { 
            println!("Calling Box Blur with kernel size {kernel_size}...");
            new_image = process_algorithms::box_blur(image, kernel_size);
        },
        ProcessType::Identity => new_image = process_algorithms::identity(image)
    }
    
    // output image!
    let write_result = file_io::write_image(
        collector.output_filename, 
        bm_header, 
        bm_info_header, 
        new_image
    ); 
    if write_result.is_err() {
        println!("Error writing image: {:?}", write_result.unwrap_err());
    }
    if collector.debug_flag {
        println!("Finished writing image in {:?}", now.elapsed());
    }
}