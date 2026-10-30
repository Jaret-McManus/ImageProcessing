use std::{
    collections::VecDeque, env, process::exit
};
mod arg_collector;
use crate::arg_collector::{ArgCollector, ProcessType};

#[derive(Debug)]
enum ProgramError{
    NotImplemented,
    InputFileError, OutputFileError
}

fn main() {
    let args: VecDeque<String> = env::args().skip(1).collect();

    let collector_result = ArgCollector::create(args);
    let Ok(collector) = collector_result else {
        println!("Error collecting arguments: {:?}", collector_result.unwrap_err());
        exit(1);
    };

    match collector.process_type {
        ProcessType::BoxBlur(kernel_size) => { 
            println!("Calling Box Blur with kernel size {kernel_size}...");
            println!("Error: {:?}", ProgramError::NotImplemented);
            exit(-1)
        }
    }

    // let _input_file_contents = fs::read_to_string(collector.input_filename)
    //     .expect("Unable to open file!");
}