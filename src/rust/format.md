./imageprocessing [options] {filenames}

option:
    --box-blur num

filenames (combination of either order input->output or output->input and short or long flags):
    -i input_filename -o output_filename
    -i input_filename --output output_filename
    --input input_filename -o output_filename
    --input input_filename --output output_filename
    
    -o output_filename -i input_filename
    -o output_filename --input input_filename
    --output output_filename -i input_filename
    --output output_filename --input input_filename