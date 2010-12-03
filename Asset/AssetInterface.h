// For conditions of distribution and use, see copyright notice in license.txt
///\deprecated The contents of this file is deprecated and will be removed in the future in favor of the new Asset API. See IAsset.h instead. -jj.

#ifndef incl_Interfaces_AssetInterface_h
#define incl_Interfaces_AssetInterface_h

#include "AssetMetadataInterface.h"
#include "CoreModuleApi.h"
#include "CoreTypes.h"
#include "AssetFwd.h"

#include <QMetaType>

namespace Foundation
{
    //! Interface for assets
    ///\deprecated This object is deprecated and will be removed in the future in favor of the new Asset API. See IAsset.h instead. -jj.
    class MODULE_API AssetInterface
    {
    public:
        //! Default constructor
        AssetInterface() {}

        //! Default destructor
        virtual ~AssetInterface() {}

        //! Returns asset id. Can be for example UUID in text form
        virtual const std::string& GetId() const = 0;

        //! Returns asset type in text form
        virtual const std::string& GetType() const = 0;

        //! Returns asset data size
        virtual uint GetSize() const = 0;

        //! Returns asset data
        virtual const u8* GetData() const = 0;

        //! Returns asset metadata
        virtual AssetMetadataInterface* GetMetadata() const = 0;
    };
}

#endif
