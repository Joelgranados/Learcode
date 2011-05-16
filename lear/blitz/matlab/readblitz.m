function [data,lb,ex] = readblitz(filename)

if isstr(filename)
    fid = fopen(filename,'rb');
    if fid ==-1
        error ['Unable to open file', filename];
    end
else
    fid = filename;
end

id=fread(fid,1,'uchar'); %vec dimension
typeid = id2str(id);

dim=fread(fid,1,'int'); %vec dimension

lb=fread(fid,dim,'int'); %vec lbound
ex=fread(fid,dim,'int'); %vec extent

lb = reshape(lb,[1, dim]);
ex = reshape(ex,[1, dim]);
s = prod(ex);

data= fread(fid,s,typeid);

if length(ex)>1
    data =reshape(data,ex(end:-1:1));
end
data = data';

if isstr(filename)
    fclose(fid);
end

