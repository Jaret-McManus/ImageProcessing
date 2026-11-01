use std::{collections::VecDeque, convert::TryFrom};
use self::ArgCollectError::{
    InvalidFlagPassed, 
    NoProcessTypeProvided,
    CannotConvertNumber, NoNumberProvided,
    // OutputFlagBeforeInput, 
    NoInputFilenameProvided, NoOutputFilenameProvided, 
};

use crate::arg_collector::ProcessType::Identity;

#[derive(Debug)]
pub enum ProcessType {
    BoxBlur(u32),
    Identity
}

#[derive(Debug)]
#[allow(dead_code)]
pub enum ArgCollectError {
    CannotConvertNumber(String),
    NoNumberProvided(String),
    InvalidFlagPassed(String),
    NoProcessTypeProvided,
    NoInputFilenameProvided, NoOutputFilenameProvided,
    NotImplemented
}

#[derive(Debug)] 
pub struct ArgCollector {
    pub process_type: ProcessType,
    pub input_filename: String,
    pub output_filename: String,
    pub debug_flag: bool,
    pub print_headers_flag: bool
}

impl ArgCollector {
    pub fn create(mut args: VecDeque<String>) -> Result<Self, ArgCollectError> {
        // collect options
        let mut process_type: Option<ProcessType> = None;
        let mut input_filename: String = "".to_string();
        let mut output_filename: String = "".to_string();
        let mut input_flag_seen: bool = false;
        let mut output_flag_seen: bool = false;
        let mut debug_flag = false;
        let mut print_headers_flag = false;

        while let Some(value) = args.pop_front() {
            match value.as_str() {
                "--box-blur" => {
                    let error_info = "box blur kernel value";
                    let number = ArgCollector::consume_next_arg_as_number(
                        args.pop_front(), error_info
                    )?;
                    let number = u32::try_from(number)
                        .map_err(|_| CannotConvertNumber(format!("{number}")))?;
                    process_type = Some(ProcessType::BoxBlur(number));
                },
                "--identity" => process_type = Some(Identity),
                "-i" | "--input" => { 
                    input_flag_seen = true;
                    input_filename = args.pop_front()
                        .ok_or(NoInputFilenameProvided)?;
                },
                "-o" | "--output" => { 
                    output_flag_seen = true;
                    output_filename = args.pop_front()
                        .ok_or(NoOutputFilenameProvided)?;
                },
                "-d" | "--debug" => debug_flag = true,
                "--print-headers" => print_headers_flag = true,
                _ => return Err( InvalidFlagPassed(value) )
            }
        }
        
        if !input_flag_seen { return Err(NoInputFilenameProvided); }
        if !output_flag_seen { return Err(NoOutputFilenameProvided); }
        
        let Some(process_type) = process_type else {
            return Err(NoProcessTypeProvided);
        };

        Ok(ArgCollector { 
            process_type, input_filename, output_filename, 
            debug_flag, print_headers_flag
        })
    }

    fn consume_next_arg_as_number(next_arg: Option<String>, error_info: &str) -> Result<i32, ArgCollectError> {
        let number= next_arg
            .ok_or( NoNumberProvided(error_info.to_string()) )
            .and_then(
                |arg| arg.parse::<i32>()
                    .map_err( |_| CannotConvertNumber(error_info.to_string()) )
            )?;
        
        Ok(number)
    }

    // fn consume_next_arg(next_arg: Option<String>, error: ArgCollectError) -> Result<String, ArgCollectError> {
    //     next_arg.ok_or(error)
    // }
    
}