use std::{
    collections::VecDeque, env, fs, process::exit
};
mod arg_collector;
use crate::arg_collector::{ArgCollector};

fn main() {
    let args: VecDeque<String> = env::args().skip(1).collect();

    let collector_result = ArgCollector::create(args);
    let Ok(collector) = collector_result else {
        println!("Error collecting arguments: {:?}", collector_result.unwrap_err());
        exit(1);
    };
    let _input_file_contents = fs::read_to_string(collector.input_filename)
        .expect("Unable to open file!");
}