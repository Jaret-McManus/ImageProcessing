use std::collections::VecDeque;
use ArgCollectError::{
    InvalidFlagPassed, 
    NoProcessTypeProvided,
    CannotConvertNumber, NoNumberProvided,
    OutputFlagBeforeInput, 
    NoInputFilenameProvided, NoOutputFilenameProvided, 
};

#[derive(Debug)]
pub enum ProcessType {
    BoxBlur(i32)
}

#[derive(Debug)]
pub enum ArgCollectError {
    CannotConvertNumber(String),
    NoNumberProvided(String),
    InvalidFlagPassed(String),
    NoProcessTypeProvided,
    OutputFlagBeforeInput,
    NoInputFilenameProvided, NoOutputFilenameProvided
}

#[derive(Debug)] 
pub struct ArgCollector {
    pub process_type: ProcessType,
    pub input_filename: String,
    pub output_filename: String
}

impl ArgCollector {
    pub fn create(mut args: VecDeque<String>) -> Result<Self, ArgCollectError> {
        // collect options
        let mut process_type: Option<ProcessType> = None;
        let mut _input_flag_seen: bool = false;
        while let Some(value) = args.pop_front() {
            match value.as_str() {
                "--box-blur" => {
                    process_type = Some(ProcessType::BoxBlur(3));
                    let error_info = "box blur kernel";
                    let number= args.pop_front()
                        .ok_or( NoNumberProvided(error_info.to_string()) )
                        .and_then(
                            |arg| arg.parse::<i32>()
                                .map_err( |_| CannotConvertNumber(error_info.to_string()) )
                        )?;
                    println!("box blur: {number}");
                },
                "-i" | "--input" => { _input_flag_seen = true; break; }
                "-o" | "--output" => return Err(OutputFlagBeforeInput),
                _ => return Err( InvalidFlagPassed(value) )
            }
        }
        let Some(_process_type) = process_type else {
            return Err(NoProcessTypeProvided);
        };

        return Err(NoInputFilenameProvided);

        // return Err(NoOutputFilenameProvided);

        // return Ok(ArgCollector { 
        //     process_type: process_type, 
        //     input_filename: "temp_input".to_owned(), 
        //     output_filename: "temp_output".to_owned() 
        // })
    }

    
}