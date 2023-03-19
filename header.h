#include <iostream>
#include <ctime>
#include <string>
#include <fstream>

using namespace std;



class Image{

    private:
        int row;
        int col;
        int** pixels;
        int kernel_size;
        int* mean_compression_arr;
        int row_for_mean;
        int col_for_mean;
        int run_lenght_arr_size;
        int** color_shift_pixels;

        //methods
        void set_row(int row);
        void set_col(int col);
        void set_pixels(int** pixels);
        void set_kernel_size(int kernel_size);
        void set_mean_compression_arr(int* mean_compression_arr);
        void set_row_for_mean();
        void set_col_for_mean();
        void set_run_lenght_arr_size(int run_lenght_arr_size);
        void set_color_shift_pixels(int** color_shift_pixels);
        int convert_to_divisable_to_kernel(int num);
        int** finding_kernel(int start_point_r, int start_point_c);
        int finding_mean_of_kernel(int** mat);
        void write_compressed_img_to_file(string file_name, int arr_size);
        int* read_compressed_img_from_file(string file_name, int &r, int &c, int &ker);
        void write_to_kernel(int** mat, int kernel_size, int start_point_r, int start_point_c, int value);
        void write_pgm(string file_name, int** mat, int row, int col);
        int** color_shift(int rate);
        void run_length_write_to_file(string file_name, int* arr);
        int* run_lenght_file_reader(string file_name, int &r, int &c);

    public:
        Image();
        Image(string file_name);
        ~Image();
        int get_row();
        int get_col();
        int** get_pixels();
        int get_kernel_size();
        int* get_mean_compression_arr();
        void read_pgm(string file_name);
        void mean_compression(string file_name, int kernel_rate = 3);
        void mean_decoder(string file_name, string file_name_to_write);
        int** binary_image_converter();
        void binary_run_lenght_encoding(string file_name, int rate = 4);
        void run_lenght_decoder(string file_name, string file_name_to_write);
};





//constractor
Image::Image() {
    cout << "IMAGE CREATED\n\n\n";
    set_row(0);
    set_col(0);
}
Image::Image(string file_name) {
    cout << "IMAGE CREATED\n\n\n";
    set_row(0);
    set_col(0);
    read_pgm(file_name);
}



//private methods
void Image::set_row(int row) {
    this->row = row;
}
void Image::set_col(int col){
    this->col = col;
}
void Image::set_pixels(int** pixels){
    this->pixels = pixels;
}
void Image::set_kernel_size(int kernel_size){
    this->kernel_size = kernel_size;
}
int Image::convert_to_divisable_to_kernel(int num) {
    while(num % kernel_size != 0){
        num --;
    }
    return num;
}
void Image::set_row_for_mean(){
    this->row_for_mean = convert_to_divisable_to_kernel(row);
}
void Image::set_col_for_mean(){
    this->col_for_mean = convert_to_divisable_to_kernel(col);
}
void Image::set_mean_compression_arr(int* mean_compression_arr) {
    this->mean_compression_arr = mean_compression_arr;
}
void Image::set_run_lenght_arr_size(int run_lenght_arr_size) {
    this->run_lenght_arr_size = run_lenght_arr_size;
}
void Image::set_color_shift_pixels(int** color_shift_pixels) {
    this->color_shift_pixels = color_shift_pixels;
}
//public methods
int Image::get_row() {
    return row;
}
int Image::get_col() {
    return col;
}
int** Image::get_pixels() {
    return pixels;
}
int Image::get_kernel_size() {
    return kernel_size;
}
int* Image::get_mean_compression_arr(){
    return mean_compression_arr;
}



//private methods
int** Image::finding_kernel(int start_point_r, int start_point_c) {

    // making kernel matrix
    int** kernel_mat = new int*[kernel_size];
    for (int i = 0; i < kernel_size; i++) {
        kernel_mat[i] = new int[kernel_size];
    }

    for (int i = 0; i < kernel_size; i++) {

        for (int j = 0; j < kernel_size; j++) {

            kernel_mat[i][j] = pixels[start_point_r][start_point_c];
            start_point_c++;
        }
        start_point_c -= kernel_size;
        start_point_r++;
    }

    return kernel_mat;
}
int Image::finding_mean_of_kernel(int** mat) {

    int sum = 0;

    for (int i = 0; i < kernel_size; i++) {

        for (int j = 0; j < kernel_size; j++) {

            sum += mat[i][j];
        }
    }


    // delete heap memory for mat. it is set from "mean_compression" and "finding_kernel" as "kernel_matrix" and "kernel_mat"
    for (int i = 0; i < kernel_size; i++) {
        delete[] mat[i];  // Deallocate each row
    }
    delete[] mat;



    return sum/(kernel_size * kernel_size);
}
void Image::write_compressed_img_to_file(string file_name, int arr_size) {


    char* c_arr = new char[arr_size];

    for (int i = 0; i < arr_size; i++) {
        c_arr[i] = (char)mean_compression_arr[i];
    }

    string s_row = to_string(row_for_mean);
    s_row += "\n";
    string s_col = to_string(col_for_mean);
    s_col += "\n";
    string s_kernel_size = to_string(kernel_size);
    s_kernel_size += "\n";




    ofstream file(file_name, ios::binary);

        if(file.is_open()) {
            int row_size =  s_row.size();
            int col_size =  s_col.size();
            int k_size =  s_kernel_size.size();

            // firt 3 element of the bin data is for decoding the image(rowsize, colsize, kernelsize)
            file.write(&s_row[0], row_size); // row
            file.write(&s_col[0], col_size); // col
            file.write(&s_kernel_size[0], k_size);   // kernel size

            for (int i = 0; i < arr_size; i++) {
                file.write(&c_arr[i], sizeof(char));
            }

            file.close();
            cout << "file succesfully saved !!!\n";
        } else {
            cout << "file did not open and saved !!!\n";
        }

    //deleting heap memory for "c_arr"
    delete[] c_arr;

}
int* Image::read_compressed_img_from_file(string file_name, int &r, int &c, int &ker) {


    //reading the header
    int size_for_increment = 0; // this is a size for incremention of pointer because we want to pass the header while we are reading img
    string line;
    ifstream textFile(file_name);
    if(textFile.is_open()) {
        for (int i = 0; i < 3; i++) {
            getline(textFile, line);
            if(i == 0){
                r = stoi(line);
                size_for_increment += line.size();
            }
            if(i == 1){
                c = stoi(line);
                size_for_increment += line.size();
            }
            if(i == 2){
                ker = stoi(line);
                size_for_increment += line.size();
            }
        }
        textFile.close();
    }
    size_for_increment += 3; // because in each line there is "\n" so it is also incremented by 3



    ifstream file;
    file.open(file_name, ios::binary | ios::ate);

    streampos fileSize = file.tellg();

    char* tmp = new char[fileSize];   //heap memory
    file.seekg(0, ios::beg);
    file.read(tmp, fileSize);
    file.close();

    char* char_arr = new char[fileSize];  // heap memory

    char* tmp_for_delete = tmp;
    char n;
    tmp += size_for_increment;
    for (int i = 3; i < fileSize; i++) {

        n = *tmp;

        char_arr[i] = n;
        tmp += sizeof(char);
    }


    int* int_arr = new int[fileSize];   //heap memory

    int k;
    for (int i = 3; i < fileSize; i++) {
        k = (int)char_arr[i];
        if(k < 0) {
            k = 256 + k;
        }

        int_arr[i] = k;
    }

    //deleting heap memory
    delete[] tmp_for_delete;
    delete[] char_arr;


    return int_arr;

}
void Image::write_to_kernel(int** mat, int kernel_size, int start_point_r, int start_point_c, int value) {

    for (int i = 0; i < kernel_size; i++) {

        for (int j = 0; j < kernel_size; j++) {

            mat[start_point_r][start_point_c] = value;
            start_point_c++;
        }
        start_point_c -= kernel_size;
        start_point_r++;
    }
}
void Image::write_pgm(string file_name, int** mat, int _row, int _col) {

    ofstream file(file_name, ios::binary);

    if(file.is_open()) {

        string s_row = to_string(_row);
        s_row += "\n";
        string s_col = to_string(_col);
        s_col += " ";
        s_col += s_row;
        int size_of_s_col = s_col.size();

        file.write("P5\n", 3); // Magic number
        file.write(&s_col[0], size_of_s_col); // Image width and height
        file.write("255\n", 4); // Max grayscale value

        char tmp;
        for (int i = 0; i < _row; i++) {

            for (int j = 0; j < _col; j++) {

                tmp = (char)mat[i][j];
                file.write(&tmp, sizeof(char));
            }

        }

        file.close();
        cout << "file succesfully saved (write_pgm) !!!\n";
    } else {
        cout << "file did not open and saved !!!\n";
    }

    //delete heap memory for mat which is created in "run_lenght_decoder" function
    for (int i = 0; i < row; i++) {
        delete[] mat[i];  // Deallocate each row
    }
    delete[] mat;

}
int** Image::color_shift(int rate){

    int** mat = new int*[row*col];
    for (int i = 0; i < row; i++) {
        mat[i] = new int[col];
    }


    for (int i = 0; i < row; i++) {

        for (int j = 0; j < col; j++) {

            mat[i][j] = (pixels[i][j]/rate) * rate;
        }
    }

    return mat;
}
void Image::run_length_write_to_file(string file_name, int* arr) {

    char* c_arr = new char[run_lenght_arr_size];   //heap memory

    for (int i = 0; i < run_lenght_arr_size; i++) {
        c_arr[i] = (char)arr[i];
    }

    string s_row = to_string(row);
    s_row += "\n";
    string s_col = to_string(col);
    s_col += "\n";

    ofstream file(file_name, ios::binary);
    if(file.is_open()) {
        int row_size =  s_row.size();
        int col_size =  s_col.size();

        // first 3 element of the bin data is for decoding the image(rowsize, colsize)
        file.write(&s_row[0], row_size); // row
        file.write(&s_col[0], col_size); // col

        for (int i = 0; i < run_lenght_arr_size; i++) {
            file.write(&c_arr[i], sizeof(char));
        }

        file.close();
        cout << "file succesfully saved for run_length!!!\n";
    } else {
        cout << "file did not open and saved !!!\n";
    }

    //delete heap memory
    delete[] c_arr;

    //delete heap memory 'arr' from function "binary_run_lenght_encoding"
    delete[] arr;

}
int* Image::run_lenght_file_reader(string file_name, int &r, int &c) {



    //reading the header
    int size_for_increment = 0; // this is a size for incremention of pointer because we want to pass the header while we are reading img
    string line;
    ifstream textFile(file_name);
    if(textFile.is_open()) {
        for (int i = 0; i < 2; i++) {
            getline(textFile, line);
            if(i == 0){
                r = stoi(line);
                size_for_increment += line.size();
            }
            if(i == 1){
                c = stoi(line);
                size_for_increment += line.size();
            }
        }
        textFile.close();
    }
    size_for_increment += 2; // because in each line there is "\n" so it is also incremented by 2



    ifstream file;
    file.open(file_name, ios::binary | ios::ate);

    streampos file_size = file.tellg();

    char* tmp = new char[file_size];  //heap memory
    file.seekg(0, ios::beg);
    file.read(tmp, file_size);
    file.close();

    char* char_arr = new char[file_size];  //heap memory


    char* tmp_for_delete = tmp;
    char n;
    tmp += size_for_increment;
    for (int i = 0; i < file_size; i++) {

        n = *tmp;

        char_arr[i] = n;
        tmp += sizeof(char);
    }

    int* int_arr = new int[file_size];   //heap memory

    int k;
    for (int i = 0; i < file_size; i++) {
        k = (int)char_arr[i];
        if(k < 0) {
            k = 256 + k;
        }
        int_arr[i] = k;
    }


    //delete heap memory
    delete[] tmp_for_delete;
    delete[] char_arr;

    return int_arr;
}


//public methods
void Image::read_pgm(string file_name) {


    //reading the header
    string line;
    ifstream textFile(file_name);
    if(textFile.is_open()) {
        for (int i = 0; i < 2; i++) {

            getline(textFile, line);
        }
        textFile.close();
    } else {
        cout << "text did not open";
    }
    string s_row;
    string s_col;

    for (int i = 0; i < line[i]; i++) {
        if(line[i] == ' '){
            line.erase(line.begin() + 0, line.end() - 4);
            break;
        }
        s_col += line[i];
    }
    s_row = line;

    //casting s_row and s_col to int
    int row = stoi(s_row);
    int col = stoi(s_col);

    int size_for_increment = s_row.size() + s_col.size() + 1; /// 1 is added because between row and col there is empty spacd



    //making matrix for image
    int** mat = new int*[row];        //heap memory
    for (int i = 0; i < row; i++) {
        mat[i] = new int[col];
    }

    //filling the matrix
    char* memory_file_for_delete;
    ifstream file(file_name,  ios::binary | ios::ate);
    if(file.is_open()){
        streampos file_size = file.tellg();
        char* memory_file = new char[file_size];    //heap memory
        file.seekg(0, ios::beg);
        file.read(memory_file, file_size);
        file.close();

        memory_file_for_delete = memory_file;
        size_for_increment += 2 + 3 + 3;    // adding 2 is for 'P5' and adding first 3 is for '255' and adding second 3 is for every '\n'.
        memory_file += size_for_increment;  // this adding part is done by manuely because it will not change ever.

        char n;
        for (int i = 0; i < row; i++) {

            for (int j = 0; j < col; j++) {

                n = *memory_file;

                mat[i][j] = (int)n;
                memory_file += sizeof(char);
            }
        }


    } else{
        cout << "text did not open from binary read";
        }


    // in pgm file after 127, chars are stored as negative numbers so thay should be modified then returned
    for (int i = 0; i < row; i++) {

        for (int j = 0; j < col; j++) {

            if(mat[i][j] < 0) {
                int k = mat[i][j];
                mat[i][j] = k + 256;
            }

        }
    }

    set_row(row);
    set_col(col);
    set_pixels(mat);


    //delete heap memory
    delete[] memory_file_for_delete;

}
void Image::mean_compression(string file_name, int kernel_rate) {

    set_kernel_size(kernel_rate);
    set_row_for_mean();
    set_col_for_mean();



    // making kernel sum array
    int arr_size = (row_for_mean/kernel_size) * (col_for_mean/kernel_size);
    int* kernel_sum = new int[arr_size];   //heap memory


    // 1-) take whole image and find kernel for each of them
        int kernel_sum_counter = 0;


        for (int i = 0; i < row_for_mean; i+=kernel_size) {

            for (int j = 0; j < col_for_mean; j+=kernel_size) {

                int** kernel_matrix = finding_kernel(i, j);

                // 2-) take the mean of the kernels and put each sum in to array and return it at the end

                kernel_sum[kernel_sum_counter] = finding_mean_of_kernel(kernel_matrix);
                kernel_sum_counter++;
            }
        }

        set_mean_compression_arr(kernel_sum);
        write_compressed_img_to_file(file_name, arr_size);
}
void Image::mean_decoder(string file_name, string file_name_to_write) {

    int _row;
    int _col;
    int _kernel_size;
    int* arr = read_compressed_img_from_file(file_name, _row, _col, _kernel_size);




    int arr_size = (_row/_kernel_size) * (_col/_kernel_size);

    int** mat = new int*[_row*_col];   //heap memory
    for (int i = 0; i < _row; i++) {
        mat[i] = new int[_col];
    }


    for (int i = 3; i < arr_size + 3; ) {

        for (int j = 0 ; j < _row; j+=_kernel_size) {

            for (int k = 0; k < _col; k+=_kernel_size) {

                write_to_kernel(mat, _kernel_size, j, k, arr[i]);
                i++;
            }
        }
    }


    write_pgm(file_name_to_write, mat, _row, _col);

    //delete heap memory for arr. this is array comes from read_compressed_img_from_file as "int_arr"
    delete[] arr;

}
int** Image::binary_image_converter(){

    int** mat = new int*[row*col];   //heap memory
    for (int i = 0; i < row; i++) {
        mat[i] = new int[col];
    }

    for (int i = 0; i < row; i++) {

        for (int j = 0; j < col; j++) {

            if(pixels[i][j] < 128){
                mat[i][j] = 0;
            } else {
                mat[i][j] = 255;
            }

        }
    }

    //dont delete this mat because it should be deleted by user in main function it this function is used. it is not mandotory for project


    return mat;
}
void Image::binary_run_lenght_encoding(string file_name, int rate) {

    //color shift
    int** color_shifted = color_shift(rate);
    set_color_shift_pixels(color_shifted);


    //making array for return
    int* arr = new int[row * col * 2];     //heap memory
    //giving arr all (-1)
    for (int i = 0; i < row * col * 2; i++) {
        arr[i] = -1;
    }



    //first pixel of pixels
    int current_pixel = color_shift_pixels[0][0];

    int repeat_count = 0;
    int count = 0;

    for (int i = 0; i < row; i++) {

        for (int j = 0; j < col; j++) {

            if(current_pixel ==  color_shift_pixels[i][j]){
                repeat_count++;
                continue;
            }

            arr[count] = repeat_count;
            arr[count + 1] = current_pixel;
            count += 2;
            repeat_count = 1;
            current_pixel = color_shift_pixels[i][j];

        }

    }

    //finding real size
    int real_size = 0;
    for (int i = 0; i < row * col * 2; i++) {
        if(arr[i] != -1){
            real_size++;
        }
    }

    //making an array with real size
    int* real_size_arr = new int[real_size];    //heap memory

    //filling real_size_arr
    for (int i = 0; i < real_size; i++) {
        real_size_arr[i] = arr[i];
    }

    //delete heap memory
    delete[] arr;

    set_run_lenght_arr_size(real_size);
    run_length_write_to_file(file_name, real_size_arr);
}
void Image::run_lenght_decoder(string file_name, string file_name_to_write){

    int _row;
    int _col;
    int* arr = run_lenght_file_reader(file_name, _row, _col);

    int** mat = new int*[_row];             //heap memory
    for (int i = 0; i < _row; i++) {
        mat[i] = new int[_col];
    }



    int count = arr[0];
    int value = arr[0 + 1];
    int index = 0;
    int count_sum = 0;

    for (int i = 0; i < _row; i++) {
        for (int j = 0; j < _col; j++) {

            if(count > 0){
                mat[i][j] = value;
                count--;
                continue;
            }
            index += 2; // it is increased by two because one is count other one is value
            count = arr[index];
            count_sum += count;
            value = arr[index + 1];
            mat[i][j] = value;
            count--;
        }
    }
    write_pgm(file_name_to_write, mat, _row, _col);

    //delete heap memory for arr. arr is in heap memory because "run_lenght_file_reader" function returns "int_arr"
    delete[] arr;
}


//destructor
Image::~Image() {
    //delete the heap memory

    // this is set from mean_compression function as "kernel_sum"
    delete [] mean_compression_arr;


    // this is set from read_pgm fuction as "mat"
    for (int i = 0; i < row; i++) {
        delete[] pixels[i];  // Deallocate each row
    }
    delete[] pixels;


    //this is set from "color_shift" and "binary_run_lenght_encoding" function
    for (int i = 0; i < row; i++) {
        delete[] color_shift_pixels[i];  // Deallocate each row
    }
    delete[] color_shift_pixels;

}
