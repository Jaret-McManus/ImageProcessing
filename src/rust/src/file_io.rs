use std::{io::Error, fs};

use crate::data_structures::Image;

#[derive(Debug)]
#[allow(dead_code)]
pub enum FileIOError {
    CannotOpenInputFile(Error),
    NotImplemented
}

pub fn read_file_to_image(input_filename: String) -> Result<Image, FileIOError> {
    let _file_contents = fs::read_to_string(input_filename)
        .map_err(|err| FileIOError::CannotOpenInputFile(err))?;

    return Err(FileIOError::NotImplemented);
}