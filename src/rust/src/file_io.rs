use std::{collections::VecDeque, fs, io::{Error, Read, Seek, SeekFrom, Write}};

use crate::{data_structures::{BitmapHeader, BitmapInfoHeader, Image, Pixel24}, file_io::FileIOError::{FailedReadEntirePixelArray, FailedReadPixel, FailedSkipByte}};

use self::FileIOError::{
    CannotOpenInputFile, 
    CannotOpenOutputFile,
    InvalidBitmapHeader,
    InvalidBitmapInfoHeader,
    FileSeekError,
    FailedReadPixelRow,
    WriteError,
    FailedReadU32, NotEnoughReadU32,
    FailedReadU16,NotEnoughReadU16,
    FailedReadU8, NotEnoughReadU8,
    // NotImplemented,
};

const BITMAP_MAGIC_NUMBER: [u8; 2] = [0x42, 0x4D];

#[derive(Debug)]
#[allow(dead_code)]
pub enum FileIOError {
    CannotOpenInputFile(Error),
    CannotOpenOutputFile(Error),
    InvalidBitmapHeader(String),
    InvalidBitmapInfoHeader(String),
    FileSeekError(Error),
    NotImplemented,
    FailedReadEntirePixelArray(Error),
    FailedReadPixelRow(String),
    FailedReadU32(Error), NotEnoughReadU32,
    FailedReadU16(Error),NotEnoughReadU16,
    FailedReadU8(Error), NotEnoughReadU8,
    FailedReadPixel,
    FailedSkipByte,
    WriteError(Error)
}

pub fn read_file_to_image(input_filename: String) 
    -> Result<(Image, BitmapHeader, BitmapInfoHeader), FileIOError> 
{
    let mut file = fs::File::open(input_filename)
        .map_err(|err| CannotOpenInputFile(err))?;

    if !is_valid_bitmap_magic_number(&mut file) { 
        return Err(InvalidBitmapHeader(format!("Invalid Magic Number for Bitmap!")));
    }

    let bm_header = read_bitmap_header(&mut file)
        .map_err(|err| InvalidBitmapHeader(format!("{:?}", err)))?;
    let bm_info_header = read_bitmap_info_header(&mut file)
        .map_err(|err| InvalidBitmapInfoHeader(format!("{:?}", err)))?;

    let image = read_image(&bm_header, &bm_info_header, file)?;
    Ok((image, bm_header, bm_info_header))
}

fn read_image(
    bm_header: &BitmapHeader, bm_info_header: &BitmapInfoHeader, mut file: fs::File
) -> Result<Image, FileIOError> {
    let mut image = Image::empty(
        bm_info_header.width as usize, bm_info_header.height as usize
    );

    file.seek(SeekFrom::Start(bm_header.offset as u64))
        .map_err(|err| FileSeekError(err))?;
    
    let pixel_array_size = (bm_header.size - bm_header.offset) as usize;
    let mut bytes = read_rest_as_bytes(pixel_array_size, &mut file)?;
    for _ in 0..bm_info_header.height {
        let row = read_pixel_row_from_bytes(bm_info_header.width, &mut bytes)?;
        image.push_row(row);
    }


    Ok(image)
}

fn is_valid_bitmap_magic_number(file: &mut fs::File) -> bool {
    let mut first_two_char = [0u8; 2];
    let bytes_read = file.read(&mut first_two_char);
    match bytes_read {
        Err(_) => return false,
        Ok(bytes_read) => if bytes_read < 2 { return false }
    }
    
    first_two_char[0] == BITMAP_MAGIC_NUMBER[0] && 
    first_two_char[1] == BITMAP_MAGIC_NUMBER[1]
}

fn read_bitmap_header(file: &mut fs::File) -> Result<BitmapHeader, FileIOError> {
    BitmapHeader::try_create(
        read_u32(file),
        read_u16(file),
        read_u16(file),
        read_u32(file),
    )
}

fn read_bitmap_info_header(file: &mut fs::File) -> Result<BitmapInfoHeader, FileIOError> {
    BitmapInfoHeader::try_create(
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u16(file),
        read_u16(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
    )
}

fn read_rest_as_bytes(expected_read_size: usize, file: &mut fs::File) -> Result<VecDeque<u8>, FileIOError> {
    let mut buf: Vec<u8> = Vec::with_capacity(expected_read_size);
    let bytes_read = file.read_to_end(&mut buf)
        .map_err(|err| FailedReadEntirePixelArray(err))?;
    
    if bytes_read != expected_read_size { 
        return Err(FailedReadPixelRow(format!(
            "Not enough bytes read: read {bytes_read} vs expected {expected_read_size}"
        )));
    }

    Ok(VecDeque::from(buf))
}

fn read_pixel_row_from_bytes(width: u32, bytes: &mut VecDeque<u8>) -> Result<Vec<Pixel24>, FileIOError> {
    let mut row = Vec::<Pixel24>::with_capacity(width as usize);
    for _ in 0..width {
        let pixel = read_pixel_from_bytes(bytes)?;
        row.push(pixel);
    }

    let bytes_read = width as usize * 3;
    let remainder_bytes = bytes_read % 4;
    if remainder_bytes != 0 {
        skip_padding_bytes(4 - remainder_bytes, bytes)?;
    }

    Ok(row)
}

fn skip_padding_bytes(padding_len: usize, bytes: &mut VecDeque<u8>) -> Result<(), FileIOError> {
    for _ in 0..padding_len {
        bytes.pop_front().ok_or(FailedSkipByte)?;
    }
    Ok(())
}

fn read_pixel_from_bytes(bytes: &mut VecDeque<u8>) -> Result<Pixel24, FileIOError> {
    let blue = bytes.pop_front().ok_or(FailedReadPixel)?;
    let green = bytes.pop_front().ok_or(FailedReadPixel)?;
    let red = bytes.pop_front().ok_or(FailedReadPixel)?;

    Ok(Pixel24::new(red, green, blue))
}

fn read_u32(file: &mut fs::File) -> Result<u32, FileIOError> {
    let mut four_bytes = [0u8; 4];
    let bytes_read = file.read(&mut four_bytes)
        .map_err(|err| FailedReadU32(err))?;

    if bytes_read < 4 { return Err(NotEnoughReadU32) }
    
    Ok(u32::from_le_bytes(four_bytes))
}

fn read_u16(file: &mut fs::File) -> Result<u16, FileIOError> {
    let mut two_bytes = [0u8; 2];
    let bytes_read = file.read(&mut two_bytes)
        .map_err(|err| FailedReadU16(err))?;

    if bytes_read < 2 { return Err(NotEnoughReadU16) }
    
    Ok(u16::from_le_bytes(two_bytes))
}

fn _read_u8(file: &mut fs::File) -> Result<u8, FileIOError> {
    let mut byte = [0u8; 1];
    let bytes_read = file.read(&mut byte)
        .map_err(|err| FailedReadU8(err))?;

    if bytes_read < 1 { 
        println!("Not enought read u8 at {}", file.stream_position().unwrap() );
        return Err(NotEnoughReadU8); 
    }
    Ok(u8::from_le_bytes(byte))

}

pub fn write_image(
    path: String, bm_header: BitmapHeader, bm_info_header: BitmapInfoHeader, image: Image
) -> Result<(), FileIOError> {
    let mut file = fs::File::create(path)
        .map_err(|err| CannotOpenOutputFile(err))?;

    write_bitmap_header(&bm_header, &mut file)?;
    write_bitmap_info_header(&bm_info_header, &mut file)?;

    file.seek(SeekFrom::Start(bm_header.offset as u64))
        .map_err(|err| WriteError(err))?;

    for row in image.pixels.iter().rev() {
        // dbg!(&row);
        write_pixel_row(row, &mut file)?;
    }

    Ok(())
}

fn write_bitmap_header(bm_header: &BitmapHeader, file: &mut fs::File) -> Result<(), FileIOError> {
    file.write_all(b"BM")
        .map_err(|err| WriteError(err))?;
    write_u32(bm_header.size, file)?;
    write_u16(bm_header.reserved1, file)?;
    write_u16(bm_header.reserved2, file)?;
    write_u32(bm_header.offset, file)?;
    
    Ok(())
}

fn write_bitmap_info_header(bm_info_header: &BitmapInfoHeader, file: &mut fs::File) -> Result<(), FileIOError> {
    write_u32(bm_info_header.hdr_size, file)?;
    write_u32(bm_info_header.width, file)?;
    write_u32(bm_info_header.height, file)?;
    write_u16(bm_info_header.planes, file)?;
    write_u16(bm_info_header.bits_per_pixel, file)?;
    write_u32(bm_info_header.compression, file)?;
    write_u32(bm_info_header.image_size, file)?;
    write_u32(bm_info_header.x_pixels_per_m, file)?;
    write_u32(bm_info_header.y_pixels_per_m, file)?;
    write_u32(bm_info_header.colors_used, file)?;
    write_u32(bm_info_header.important_colors, file)?;

    Ok(())
}

fn write_pixel_row(row: &Vec<Pixel24>, file: &mut fs::File) -> Result<(), FileIOError> {
    let bytes_written_per_row = row.len() * 3;
    for pixel in row {
        write_pixel(pixel, file)?;
    }
    
    let remainder_bytes = bytes_written_per_row % 4;
    if remainder_bytes != 0 {
        write_padding(4 - remainder_bytes, file)?;
    }
    Ok(())
}

fn write_pixel(pixel: &Pixel24, file: &mut fs::File) -> Result<(), FileIOError> {
    write_u8(pixel.blue, file)?;
    write_u8(pixel.green, file)?;
    write_u8(pixel.red, file)?;

    Ok(())
}

fn write_u32(number: u32, file: &mut fs::File) -> Result<(), FileIOError> {
    file.write(&number.to_le_bytes())
        .map_err(|err| WriteError(err))?;
    Ok(())
}

fn write_u16(number: u16, file: &mut fs::File) -> Result<(), FileIOError> {
    file.write(&number.to_le_bytes())
        .map_err(|err| WriteError(err))?;
    Ok(())
}

fn write_u8(number: u8, file: &mut fs::File) -> Result<(), FileIOError> {
    file.write(&number.to_le_bytes())
        .map_err(|err| WriteError(err))?;
    Ok(())

}

fn write_padding(padding_len: usize, file: &mut fs::File) -> Result<(), FileIOError> {
    for _ in 0..padding_len {
        file.write(b"\0")
            .map_err(|err| WriteError(err))?;
    }

    Ok(())
}