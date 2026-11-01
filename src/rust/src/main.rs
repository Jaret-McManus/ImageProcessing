use std::{
    collections::VecDeque, env, process::exit, time::Instant
};
mod arg_collector;
mod file_io;
mod data_structures;
mod process_algorithms;
use crate::{
    arg_collector::{ArgCollector, ProcessType}, data_structures::{BitmapHeader, BitmapInfoHeader, Image}
};

fn main() {
    let start = Instant::now();
    let args: VecDeque<String> = env::args().skip(1).collect();

    let collector_result = ArgCollector::create(args);
    let Ok(collector) = collector_result else {
        println!("Error collecting arguments: {:?}", collector_result.unwrap_err());
        exit(1);
    };

    let read_start = Instant::now();
    let image_result = file_io::read_file_to_image(collector.input_filename.clone());
    let Ok((
        image,
        bm_header,
        bm_info_header
    )) = image_result else {
        println!("Error reading image: {:?}", image_result.unwrap_err());
        exit(-1);
    };

    debug_print_read_time(&start, read_start, &collector);
    debug_print_headers(&bm_header, &bm_info_header, &collector);
    
    let new_image: Image;
    match collector.process_type {
        ProcessType::BoxBlur(kernel_size) => { 
            println!("Calling Box Blur with kernel size {kernel_size}...");
            new_image = process_algorithms::box_blur(image, kernel_size);
        },
        ProcessType::Identity => new_image = process_algorithms::identity(image)
    }
    
    // output image!
    let write_start = Instant::now();
    let write_result = file_io::write_image(
        collector.output_filename.clone(), 
        bm_header, 
        bm_info_header, 
        new_image
    ); 
    if write_result.is_err() {
        println!("Error writing image: {:?}", write_result.unwrap_err());
    }
    debug_print_write_time(&start, write_start, &collector);
}

fn debug_print_headers(bm_header: &BitmapHeader, bm_info_header: &BitmapInfoHeader, collector: &ArgCollector) -> () {
    if collector.print_headers_flag { 
        println!("{:#?}", bm_header); 
        println!("{:#?}", bm_info_header); 
    }
}

fn debug_print_read_time(start: &Instant, read_start: Instant, collector: &ArgCollector) -> () {
    if collector.debug_flag {
        let output = format!(
            "Finished reading image:\n\t{:18} = {:?}\n\t{:18} = {:?}",
            "total time elapsed", start.elapsed(),
            "time reading", read_start.elapsed()
        );
        println!("{output}");
    }
}

fn debug_print_write_time(start: &Instant, write_start: Instant, collector: &ArgCollector) -> () {
    if collector.debug_flag {
        let output = format!(
            "Finished writing image:\n\t{:18} = {:?}\n\t{:18} = {:?}",
            "total time elapsed", start.elapsed(),
            "time writing", write_start.elapsed()
        );
        println!("{output}");
    }
}