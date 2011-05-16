#include <lear/io/fileheader.h>
#include <iomanip>

using namespace lear;
FileHeader::FileHeader(const std::string& tag__ , const int version__):
            version_(version__)
{
    if (tag__.length()>= HEADER_LEN) 
        tag_=tag__.substr(0,HEADER_LEN-1);
    else 
        tag_=tag__;
}
void FileHeader::bload(BiIStream& stream){
    tag_ = readtag(stream);
    if (!stream.good())
        throw Exception("FileHeader::bload",
                "Unable to fully read file identifier tag.");

    stream >> version_;
    if (!stream.good())
        throw Exception("FileHeader::bload",
            "Corrupted file format. "
            "Unable to fully read file version number");
}

/// save FileHeader to an output stream in binary format. 
void FileHeader::bsave(BiOStream& stream) const {
    LenType len= tag_.size();
    LenType wlen = len < HEADER_LEN ? len : HEADER_LEN;

    for (LenType i= 0; i< wlen; ++i) 
        stream.put(tag_[i]);
    for (LenType i= wlen; i< HEADER_LEN; ++i) 
        stream.put('\0');

    stream << version_;
}

bool FileHeader::verify(const std::string& file) const {
    BiIStream stream(file.c_str());
    if (!stream) {
        throw Exception("FileHeader::verify",
            "File open failed. Unable to verify "
            "identity tag for file : " + file);
    }
    return verify(stream);
}
bool FileHeader::identify(const std::string& tagp) const {
    int l = tagp.length() < HEADER_LEN ? tagp.length() : HEADER_LEN-1;
    std::string t (tagp,0,l);
    return tag_ == t;
}

bool FileHeader::verify(BiIStream& stream) const {
    std::string ttag = readtag(stream);
    if (!stream.good())
        return false;
    return identify(ttag);
}
std::string FileHeader::readtag(BiIStream& stream) const {
    char ttag[HEADER_LEN+1];
    for (unsigned int i= 0; i< HEADER_LEN; ++i) 
        stream.get(ttag[i]);
    ttag[HEADER_LEN-1] ='\0';
    return ttag;
}

std::ostream& lear::operator<<(std::ostream& o, const FileHeader& f){
    std::cout << "Fileheader tag: "<< std::setw(8) << f.tag() 
        << ", version: " << std::setw(6) << f.version() << std::endl;
    return o;
}
