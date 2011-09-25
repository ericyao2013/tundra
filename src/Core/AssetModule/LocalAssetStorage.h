// For conditions of distribution and use, see copyright notice in license.txt

#pragma once

#include "AssetModuleApi.h"
#include "IAssetStorage.h"

#include <QMap>

class QFileSystemWatcher;
class AssetAPI;

/// Represents a single (possibly recursive) directory on the local file system.
class ASSET_MODULE_API LocalAssetStorage : public IAssetStorage
{
    Q_OBJECT

public:
    /// recursive, writable, liveUpdate and autoDiscoverable all set to true by default.
    LocalAssetStorage();
    ~LocalAssetStorage();

    /// Specifies the absolute path of the storage.
    QString directory;

    /// Specifies a human-readable name for this storage.
    QString name;

    /// If true, all subdirectories of the storage directory are automatically looked in when loading an asset.
    bool recursive;

    /// If true, assets can be written to the storage.
    bool writable;

    /// If true, assets in this storage are subject to live update after loading.
    bool liveUpdate;
    
    /// If true, storage has automatic discovery of new assets enabled.
    bool autoDiscoverable;
    
    /// Starts listening on the local directory this asset storage points to.
    void SetupWatcher();

    /// Stops and deallocates the directory change listener.
    void RemoveWatcher();

    /// Load all assets of specific suffix
    void LoadAllAssetsOfType(AssetAPI *assetAPI, const QString &suffix, const QString &assetType);

    ///\todo Evaluate if could be removed. Now both AssetAPI and LocalAssetStorage manage list of asset refs.
    QStringList assetRefs;

    QFileSystemWatcher *changeWatcher;

public slots:
    /// Specifies whether data can be uploaded to this asset storage.
    virtual bool Writable() const { return writable; }

    /// Specifies whether the assets in the storage should be subject to live update, once loaded
    virtual bool HasLiveUpdate() const { return liveUpdate; }
    
    /// Specifies whether the asset storage has automatic discovery of new assets enabled
    virtual bool AutoDiscoverable() const { return autoDiscoverable; }
    
    /// Local storages are always assumed to be trusted.
    bool Trusted() const { return true; }

    /// Returns the full local filesystem path name of the given asset in this storage, if it exists.
    /// Example: GetFullPathForAsset("my.mesh", true) might return "C:\Projects\Tundra\bin\data\assets".
    /// If the file does not exist, returns "".
    QString GetFullPathForAsset(const QString &assetname, bool recursive);

    /// Returns the URL that should be used in a scene asset reference attribute to refer to the asset with the given localName.
    /// Example: GetFullAssetURL("my.mesh") might return "local://my.mesh".
    /// @note LocalAssetStorage ignores all subdirectory specifications, so GetFullAssetURL("data/assets/my.mesh") would also return "local://my.mesh".
    QString GetFullAssetURL(const QString &localName);
    
    /// Returns the type of this storage: "LocalAssetStorage".
    virtual QString Type() const;

    /// Returns all assetrefs currently known in this asset storage. Does not load the assets
    virtual QStringList GetAllAssetRefs() { return assetRefs; }
    
    /// Refresh asset refs. Issues a directory query and emits AssetRefsChanged immediately
    virtual void RefreshAssetRefs();

    QString Name() const { return name; }

    QString BaseURL() const { return "local://"; }

    /// Returns a convenient human-readable representation of this storage.
    QString ToString() const { return Name() + " (" + directory + ")"; }

    /// Serializes this storage to a string for machine transfer.
    virtual QString SerializeToString(bool networkTransfer = false) const;

    /// If @c change is IAssetStorage::AssetCreate, adds file to the list of asset refs and signal
    void EmitAssetChanged(QString absoluteFilename, IAssetStorage::ChangeType change);

private:
    Q_DISABLE_COPY(LocalAssetStorage)
};
