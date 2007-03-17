#ifndef WXSAUTORESOURCETREEIMAGE_H
#define WXSAUTORESOURCETREEIMAGE_H
/** \brief This class helps automatically loading images into resource browser */
class wxsAutoResourceTreeImage
{
    public:

        /** \brief Load image from file in data directory
         *  \param FileName name of file relative to C::B's data directory
         *  \param DontFree if true, image's index won't be freed automatically
         *         when class is deleted. Set to true only in case of some global
         *         images which will stay loaded as long as wxSmith is loaded.
         */
        wxsAutoResourceTreeImage(const wxString& FileName,bool DontFree=false);

        /** \brief Create image from given bitmap
         *  \param Image bitmap to be added
         *  \param DontFree if true, image's index won't be freed automatically
         *         when class is deleted. Set to true only in case of some global
         *         images which will stay loaded as long as wxSmith is loaded.
         */
        wxsAutoResourceTreeImage(const wxBitmap& Image,bool DontFree=false);

        /** \brief Dctor - marks image's index as free if necessary */
        ~wxsAutoResourceTreeImage();

        /** \brief Get index of handled image */
        inline int GetIndex() { return m_Index; }

    private:

        int  m_Index;
        bool m_DontFree;
};



#endif
