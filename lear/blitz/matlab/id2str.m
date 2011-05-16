function  typestr = id2str(id) 

switch id
case 1
    typestr = 'uchar';
case 2
    typestr = 'schar';
case 3
    typestr = 'int';
case 4
    typestr = 'float32';
case 5
    typestr = 'double';
case 6
    typestr = 'uchar';
case 7
    typestr = 'int16';
case 8
    typestr = 'uint16';
case 9
    typestr = 'uint32';
case 10
    typestr = 'int32';
case 11
    typestr = 'uint32';
otherwise
    error ['Unsupported type field ', num2str(id)]
end

