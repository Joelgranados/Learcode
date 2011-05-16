/*
 * =====================================================================================
 * 
 *       Filename:  rawdescio.h
 * 
 *    Description:  Provides interface to dump_rhog descriptor file IO.
 * 
 * =====================================================================================
 */

#ifndef _LEAR_RAW_DESC_IO_H_
#define _LEAR_RAW_DESC_IO_H_

#include <string>
#include <iostream>
#include <blitz/array.h> 
#include <blitz/tinyvec.h>

#include <lear/io/biistream.h>
#include <lear/io/fileheader.h>
#include <lear/cvision/densegrid.h>

typedef float                       RealType; 
typedef blitz::TinyVector<int,2>    IndexType;

typedef blitz::Array<RealType,1>    Array1DType;
typedef blitz::Array<RealType,2>    Array2DType;

class RawDescIn {
        typedef lear::DenseGrid<2>::GridType GridType;
    public:
        RawDescIn(const std::string& filename_, const int verbose=0);

        /// return the assoicated filename 
        std::string filename() const {return filename_;}

        /// number of feature vectors in this file
        int featureCount() const { return size_; }

        /// dimension of each featre vector
        int featureLength() const { return length_; }

        /// total number of different descriptors used
        int descNumber() const { return descNum_; }

        /// returns the dimension of each descriptor used
        int descLength(const int index) const { return descLength_(index); }

        /// returns the extent of each descriptor used
        IndexType descExtent(const int index) const 
        { return descExtent_(index); }

        /// returns the xy grid for each descriptor used
        const GridType& grid(const int index) const { return grid_[index]; }

        /// extent of window in pixels
        IndexType extent() const { return extent_; }

        /// Return true if call to fwinFilename would hold valid data
        bool validFilename() const { return validfilename_;}

        /// Return true if call to fwinLbound, fwinExtent, fwinScale would 
        /// hold valid data
        bool validXYInfo() const { return validxyinfo_;}

        /**
         * get feature window top-left position. 
         *
         * Must call validXYInfo() to check if the value returned was really 
         * present in the data file 
         */
        IndexType fwinLbound() const { return fwinlbound_; }

        /**
         * get feature window extent 
         *
         * Must call validXYInfo() to check if the value returned was really 
         * present in the data file 
         */
        IndexType fwinExtent() const { return fwinextent_; }

        /**
         * get feature window scale
         *
         * Must call validXYInfo() to check if the value returned was really 
         * present in the data file 
         */
        RealType fwinScale() const { return fwinscale_; }

        /**
         * get feature window filename
         *
         * Must call validFilename() to check if the value returned was really 
         * present in the data file 
         */
        std::string fwinFilename() const { return fwinfilename_; }

        /// if more feature vectors are present
        bool hasMore() const { return current < size_; }

        /// if stream is valid and hasMore vectors
        operator bool() const { return from && hasMore(); }

        /// get next feature from file
        const Array1DType& next() { return next(feature);}

        /// get next feature from file in this feature vector
        Array1DType& next(Array1DType& f);

        /**
         * Split feature vector for given index into individual vectors.
         * Array elements belonging to index are copied to return array.
         */
        Array2DType next_split(const Array1DType& f, const int index) const;
        void next_split(Array2DType& rf, const Array1DType& f, 
                const int index) const;

        /**
         * Read extra information in the feature vector. Typically
         * top-left corner, extent, scale, and image name at which 
         * feature was computed are also read.
         *
         * NOTE: following information is read only if
         * it is contained in the input file 
         */
        void readExtra();
        static const lear::FileHeader header;
//          void reset();
    private:
        const std::string filename_;

        lear::FileHeader fheader;
        lear::BiIStream from;

        lear::BiIStream::pos_type startpos;

        /// dimension of feature vector
        int length_; 

        /// window extent used to compute feature vector
        IndexType extent_; 

        bool validfilename_,validxyinfo_;
        /// Feature window lbound in original image
        IndexType fwinlbound_;
        /// Feature window extent in original image
        IndexType fwinextent_;
        /// Feature window scale in original image
        RealType fwinscale_;
        /// Feature window image file name image
        std::string fwinfilename_;
        // Feature window image content 
        //ImageType fwinimage_;

        /// number of descriptors used to estimate feature
        int                        descNum_;
        /// length of each descriptor
        blitz::Array<int,1>        descLength_;
        /// extent of each descriptor
        blitz::Array<IndexType,1>  descExtent_;
        /// Grid Vector for each descriptor
        std::vector< GridType>     grid_;

        /// number of total feature vector present in this file
        int size_;

        // number of features read till now
        int current;

        /// temp vector to store feature 
        Array1DType feature;
};

#endif // _LEAR_RAW_DESC_IO_H_
