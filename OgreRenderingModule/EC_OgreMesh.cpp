// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "Foundation.h"
#include "OgreRenderingModule.h"
#include "Renderer.h"
#include "EC_OgrePlaceable.h"
#include "EC_OgreMesh.h"
#include "RexTypes.h"

#include <Ogre.h>
#include <OgreTagPoint.h>

namespace OgreRenderer
{
    EC_OgreMesh::EC_OgreMesh(Foundation::ModuleInterface* module) :
        Foundation::ComponentInterface(module->GetFramework()),
        renderer_(checked_static_cast<OgreRenderingModule*>(module)->GetRenderer()),
        entity_(NULL),
        adjustment_node_(NULL),
        attached_(false),
        cast_shadows_(false),
        cloned_(false),
        draw_distance_(0.0)
    {
        Ogre::SceneManager* scene_mgr = renderer_->GetSceneManager();
        adjustment_node_ = scene_mgr->createSceneNode();
    }
    
    EC_OgreMesh::~EC_OgreMesh()
    {
        RemoveMesh();
        
        if (adjustment_node_)
        {
            Ogre::SceneManager* scene_mgr = renderer_->GetSceneManager();
            scene_mgr->destroySceneNode(adjustment_node_);
            adjustment_node_ = NULL;
        }
    }
    
    void EC_OgreMesh::SetPlaceable(Foundation::ComponentPtr placeable)
    {
        if (!dynamic_cast<EC_OgrePlaceable*>(placeable.get()))
        {
            OgreRenderingModule::LogError("Attempted to set placeable which is not " + NameStatic());
            return;
        }
        
        DetachEntity();
        placeable_ = placeable;
        AttachEntity();
    }
    
    void EC_OgreMesh::SetAdjustPosition(const Core::Vector3df& position)
    {
        adjustment_node_->setPosition(Ogre::Vector3(position.x, position.y, position.z));
    }

    void EC_OgreMesh::SetAdjustOrientation(const Core::Quaternion& orientation)
    {
        adjustment_node_->setOrientation(Ogre::Quaternion(orientation.w, orientation.x, orientation.y, orientation.z));
    }
    
    void EC_OgreMesh::SetAdjustScale(const Core::Vector3df& scale)
    {
        adjustment_node_->setScale(Ogre::Vector3(scale.x, scale.y, scale.z));
    }
    
    void EC_OgreMesh::SetAttachmentPosition(Core::uint index, const Core::Vector3df& position)
    {
        if (index >= attachment_nodes_.size() || attachment_nodes_[index] == 0)
            return;
        
        attachment_nodes_[index]->setPosition(Ogre::Vector3(position.x, position.y, position.z));
    }

    void EC_OgreMesh::SetAttachmentOrientation(Core::uint index, const Core::Quaternion& orientation)
    {
        if (index >= attachment_nodes_.size() || attachment_nodes_[index] == 0)
            return;
        
        attachment_nodes_[index]->setOrientation(Ogre::Quaternion(orientation.w, orientation.x, orientation.y, orientation.z));
    }
    
    void EC_OgreMesh::SetAttachmentScale(Core::uint index, const Core::Vector3df& scale)
    {
        if (index >= attachment_nodes_.size() || attachment_nodes_[index] == 0)
            return;
        
        attachment_nodes_[index]->setScale(Ogre::Vector3(scale.x, scale.y, scale.z));
    }
    
    Core::Vector3df EC_OgreMesh::GetAdjustPosition() const
    {
        const Ogre::Vector3& pos = adjustment_node_->getPosition();
        return Core::Vector3df(pos.x, pos.y, pos.z);
    }
    
    Core::Quaternion EC_OgreMesh::GetAdjustOrientation() const
    {
        const Ogre::Quaternion& orientation = adjustment_node_->getOrientation();
        return Core::Quaternion(orientation.x, orientation.y, orientation.z, orientation.w);
    }     
    
    Core::Vector3df EC_OgreMesh::GetAdjustScale() const
    {
        const Ogre::Vector3& scale = adjustment_node_->getScale();
        return Core::Vector3df(scale.x, scale.y, scale.z);
    }
    
    Core::Vector3df EC_OgreMesh::GetAttachmentPosition(Core::uint index) const
    {
        if (index >= attachment_nodes_.size() || attachment_nodes_[index] == 0)
            return Core::Vector3df(0.0f, 0.0f, 0.0f);
            
        const Ogre::Vector3& pos = attachment_nodes_[index]->getPosition();
        return Core::Vector3df(pos.x, pos.y, pos.z);
    }
    
    Core::Quaternion EC_OgreMesh::GetAttachmentOrientation(Core::uint index) const
    {
        if (index >= attachment_nodes_.size() || attachment_nodes_[index] == 0)
            return Core::Quaternion();
            
        const Ogre::Quaternion& orientation = attachment_nodes_[index]->getOrientation();
        return Core::Quaternion(orientation.x, orientation.y, orientation.z, orientation.w);
    }     
    
    Core::Vector3df EC_OgreMesh::GetAttachmentScale(Core::uint index) const
    {
        if (index >= attachment_nodes_.size() || attachment_nodes_[index] == 0)
            return Core::Vector3df(1.0f, 1.0f, 1.0f);
            
        const Ogre::Vector3& scale = attachment_nodes_[index]->getScale();
        return Core::Vector3df(scale.x, scale.y, scale.z);
    }
    
    void EC_OgreMesh::SetDrawDistance(float draw_distance)
    {
        draw_distance_ = draw_distance;
        if (entity_)
            entity_->setRenderingDistance(draw_distance_);
        for (Core::uint i = 0; i < attachment_entities_.size(); ++i)
        {
            if (attachment_entities_[i])
                attachment_entities_[i]->setRenderingDistance(draw_distance_);
        }
    }
    
    bool EC_OgreMesh::SetMesh(const std::string& mesh_name, Scene::Entity *parent_entity, bool clone)
    {
        assert (parent_entity);
        RemoveMesh();

        Ogre::SceneManager* scene_mgr = renderer_->GetSceneManager();
        
        Ogre::Mesh* mesh = PrepareMesh(mesh_name, clone);
        if (!mesh)
            return false;
        
        try
        {
            entity_ = scene_mgr->createEntity(renderer_->GetUniqueObjectName(), mesh->getName());
            if (!entity_)
            {
                OgreRenderingModule::LogError("Could not set mesh " + mesh_name);
                return false;
            }
            
            entity_->setRenderingDistance(draw_distance_);
            entity_->setCastShadows(cast_shadows_);
            entity_->setUserAny(Ogre::Any(parent_entity));
        }
        catch (Ogre::Exception& e)
        {
            OgreRenderingModule::LogError("Could not set mesh " + mesh_name + ": " + std::string(e.what()));
            return false;
        }
        
        AttachEntity();
        return true;
    }
    
    void EC_OgreMesh::RemoveMesh()
    {
        if (!entity_)
            return;

        RemoveAllAttachments();
        DetachEntity();

        std::string mesh_name = entity_->getMesh()->getName();
        Ogre::SceneManager* scene_mgr = renderer_->GetSceneManager();
        scene_mgr->destroyEntity(entity_);
        
        entity_ = NULL;
        
        if (cloned_)
        {
            cloned_ = false;
            try
            {
                Ogre::MeshManager::getSingleton().remove(mesh_name);
            }
            catch (Ogre::Exception& e)
            {
                OgreRenderingModule::LogWarning("Could not remove cloned mesh:" + std::string(e.what()));
            }
        }
    }
    
    bool EC_OgreMesh::SetAttachmentMesh(Core::uint index, const std::string& mesh_name, const std::string& attach_point, bool share_skeleton)
    {
        if (!entity_)
        {
            OgreRenderingModule::LogError("No mesh entity created yet, can not create attachments");
            return false;
        }
        
        Ogre::SceneManager* scene_mgr = renderer_->GetSceneManager();
        
        size_t oldsize = attachment_entities_.size();
        size_t newsize = index + 1;
        
        if (oldsize < newsize)
        {
            attachment_entities_.resize(newsize);
            attachment_nodes_.resize(newsize);
            for (Core::uint i = oldsize; i < newsize; ++i)
            {
                attachment_entities_[i] = 0;
                attachment_nodes_[i] = 0;
            }
        }
        
        RemoveAttachmentMesh(index);
        
        Ogre::Mesh* mesh = PrepareMesh(mesh_name, false);
        if (!mesh)
            return false;
            
        try
        {
            attachment_entities_[index] = scene_mgr->createEntity(renderer_->GetUniqueObjectName(), mesh->getName());
            if (!attachment_entities_[index])
            {
                OgreRenderingModule::LogError("Could not set attachment mesh " + mesh_name);
                return false;
            }

            attachment_entities_[index]->setRenderingDistance(draw_distance_);
            attachment_entities_[index]->setCastShadows(cast_shadows_);
            attachment_entities_[index]->setUserAny(entity_->getUserAny());
        
            Ogre::Bone* attach_bone = 0;
            if (!attach_point.empty())
            {
                Ogre::Skeleton* skel = entity_->getSkeleton();
                if (skel && skel->hasBone(attach_point))
                    attach_bone = skel->getBone(attach_point);
            }
            if (attach_bone)
            {
                Ogre::TagPoint* tag = entity_->attachObjectToBone(attach_point, attachment_entities_[index]);
                attachment_nodes_[index] = tag;
            }
            else
            {
                Ogre::SceneNode* node = scene_mgr->createSceneNode();
                node->attachObject(attachment_entities_[index]);
                adjustment_node_->addChild(node);
                attachment_nodes_[index] = node;
            }
            
            if (share_skeleton && entity_->hasSkeleton() && attachment_entities_[index]->hasSkeleton())
            {
                attachment_entities_[index]->shareSkeletonInstanceWith(entity_);
            }
        }
        catch (Ogre::Exception& e)
        {
            OgreRenderingModule::LogError("Could not set attachment mesh " + mesh_name + ": " + std::string(e.what()));
            return false;
        }
        return true;
    }
    
    void EC_OgreMesh::RemoveAttachmentMesh(Core::uint index)
    {
        if (!entity_)
            return;
            
        if (index >= attachment_entities_.size())
            return;
        
        Ogre::SceneManager* scene_mgr = renderer_->GetSceneManager();
        
        if (attachment_entities_[index] && attachment_nodes_[index])
        {
            // See if attached to a tagpoint or an ordinary node
            Ogre::TagPoint* tag = dynamic_cast<Ogre::TagPoint*>(attachment_nodes_[index]);
            if (tag)
            {
                entity_->detachObjectFromBone(attachment_entities_[index]);
            }
            else
            {
                Ogre::SceneNode* scenenode = dynamic_cast<Ogre::SceneNode*>(attachment_nodes_[index]);
                if (scenenode)
                {
                    scenenode->detachObject(attachment_entities_[index]);
                    scene_mgr->destroySceneNode(scenenode);
                }
            }
            
            attachment_nodes_[index] = 0;
        }
        if (attachment_entities_[index])
        {
            if (attachment_entities_[index]->sharesSkeletonInstance())
                attachment_entities_[index]->stopSharingSkeletonInstance();
            scene_mgr->destroyEntity(attachment_entities_[index]);
            attachment_entities_[index] = 0;
        }
    }
    
    void EC_OgreMesh::RemoveAllAttachments()
    {
        for (Core::uint i = 0; i < attachment_entities_.size(); ++i)
            RemoveAttachmentMesh(i);
        attachment_entities_.clear();
        attachment_nodes_.clear();
    }
    
    bool EC_OgreMesh::SetMaterial(Core::uint index, const std::string& material_name)
    {
        if (!entity_)
        {
            OgreRenderingModule::LogError("Could not set material " + material_name + ": no mesh");
            return false;
        }
        
        if (index >= entity_->getNumSubEntities())
        {
            OgreRenderingModule::LogError("Could not set material " + material_name + ": illegal submesh index " + Core::ToString<Core::uint>(index));
            return false;
        }
        
        try
        {
            entity_->getSubEntity(index)->setMaterialName(material_name);
        }
        catch (Ogre::Exception& e)
        {
            OgreRenderingModule::LogError("Could not set material " + material_name + ": " + std::string(e.what()));
            return false;
        }
        
        return true;
    }
    
    bool EC_OgreMesh::SetAttachmentMaterial(Core::uint index, Core::uint submesh_index, const std::string& material_name)
    {
        if (index >= attachment_entities_.size() || attachment_entities_[index] == 0)
        {
            OgreRenderingModule::LogError("Could not set material " + material_name + " on attachment: no mesh");
            return false;
        }
        
        if (submesh_index >= attachment_entities_[index]->getNumSubEntities())
        {
            OgreRenderingModule::LogError("Could not set material " + material_name + " on attachment: illegal submesh index " + Core::ToString<Core::uint>(submesh_index));
            return false;
        }
        
        try
        {
            attachment_entities_[index]->getSubEntity(submesh_index)->setMaterialName(material_name);
        }
        catch (Ogre::Exception& e)
        {
            OgreRenderingModule::LogError("Could not set material " + material_name + " on attachment: " + std::string(e.what()));
            return false;
        }
        
        return true;
    }
    
        
    void EC_OgreMesh::SetCastShadows(bool enabled)
    {
        cast_shadows_ = enabled;
        if (entity_)
            entity_->setCastShadows(cast_shadows_);
        //! \todo might want to disable shadows for some attachments
        for (Core::uint i = 0; i < attachment_entities_.size(); ++i)
        {
            if (attachment_entities_[i])
                attachment_entities_[i]->setCastShadows(cast_shadows_);
        }
    }
    
    
    Core::uint EC_OgreMesh::GetNumMaterials() const
    {
        if (!entity_)
            return 0;
            
        return entity_->getNumSubEntities();
    }
    
    Core::uint EC_OgreMesh::GetAttachmentNumMaterials(Core::uint index) const
    {
        if (index >= attachment_entities_.size() || attachment_entities_[index] == 0)
            return 0;
            
        return attachment_entities_[index]->getNumSubEntities();
    }
    
    const std::string& EC_OgreMesh::GetMaterialName(Core::uint index) const
    {
        const static std::string empty;
        
        if (!entity_)
            return empty;
        
        if (index >= entity_->getNumSubEntities())
            return empty;
        
        return entity_->getSubEntity(index)->getMaterialName();
    }
    
    const std::string& EC_OgreMesh::GetAttachmentMaterialName(Core::uint index, Core::uint submesh_index) const
    {
        const static std::string empty;
        
        if (index >= attachment_entities_.size() || attachment_entities_[index] == 0)
            return empty;
        if (submesh_index >= attachment_entities_[index]->getNumSubEntities())
            return empty;
        
        return attachment_entities_[index]->getSubEntity(submesh_index)->getMaterialName();
    }
    
    bool EC_OgreMesh::HasAttachmentMesh(Core::uint index) const
    {
        if (index >= attachment_entities_.size() || attachment_entities_[index] == 0)
            return false;
            
        return true;
    }
    
    Ogre::Entity* EC_OgreMesh::GetAttachmentEntity(Core::uint index) const
    {
        if (index >= attachment_entities_.size())
            return 0;
        return attachment_entities_[index];
    }
    
    const std::string& EC_OgreMesh::GetMeshName() const
    {
        static std::string empty_name;
        
        if (!entity_)
            return empty_name;
        else
            return entity_->getMesh()->getName();
    }
    
    void EC_OgreMesh::GetBoundingBox(Core::Vector3df& min, Core::Vector3df& max) const
    {
        if (!entity_)
        {
            min = Core::Vector3df(0.0, 0.0, 0.0);
            max = Core::Vector3df(0.0, 0.0, 0.0);
            return;
        }
     
        const Ogre::AxisAlignedBox& bbox = entity_->getMesh()->getBounds();
        const Ogre::Vector3& bboxmin = bbox.getMinimum();
        const Ogre::Vector3& bboxmax = bbox.getMaximum();
        
        min = Core::Vector3df(bboxmin.x, bboxmin.y, bboxmin.z);
        max = Core::Vector3df(bboxmax.x, bboxmax.y, bboxmax.z);
    }
    
    void EC_OgreMesh::DetachEntity()
    {
        if ((!attached_) || (!entity_) || (!placeable_))
            return;
            
        EC_OgrePlaceable* placeable = checked_static_cast<EC_OgrePlaceable*>(placeable_.get());
        Ogre::SceneNode* node = placeable->GetSceneNode();
        adjustment_node_->detachObject(entity_);
        node->removeChild(adjustment_node_);
                
        attached_ = false;
    }
    
    void EC_OgreMesh::AttachEntity()
    {
        if ((attached_) || (!entity_) || (!placeable_))
            return;
            
        EC_OgrePlaceable* placeable = checked_static_cast<EC_OgrePlaceable*>(placeable_.get());
        Ogre::SceneNode* node = placeable->GetSceneNode();
        node->addChild(adjustment_node_);
        adjustment_node_->attachObject(entity_);
                
        attached_ = true;
    }
    
    Ogre::Mesh* EC_OgreMesh::PrepareMesh(const std::string& mesh_name, bool clone)
    {
        Ogre::MeshManager& mesh_mgr = Ogre::MeshManager::getSingleton();
        Ogre::MeshPtr mesh = mesh_mgr.getByName(mesh_name);
        
        // For local meshes, mesh will not get automatically loaded until used in an entity. Load now if necessary
        if (mesh.isNull())
        {
            try
            {
                mesh_mgr.load(mesh_name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                mesh = mesh_mgr.getByName(mesh_name);
            }
            catch (Ogre::Exception& e)
            {
                OgreRenderingModule::LogError("Could not load mesh " + mesh_name + ": " + std::string(e.what()));
                return 0;
            }
        }
        
        // If mesh is still null, must abort
        if (mesh.isNull())
        {
            OgreRenderingModule::LogError("Mesh " + mesh_name + " does not exist");
            return 0;
        }
        
        if (clone)
        {
            try
            {
                mesh = mesh->clone(renderer_->GetUniqueObjectName());
            }
            catch (Ogre::Exception& e)
            {
                OgreRenderingModule::LogError("Could not clone mesh " + mesh_name + ":" + std::string(e.what()));
                return 0;
            }
        }
        
        if (mesh->hasSkeleton())
        {
            Ogre::SkeletonPtr skeleton = Ogre::SkeletonManager::getSingleton().getByName(mesh->getSkeletonName());
            if (skeleton.isNull() || skeleton->getNumBones() == 0)
            {
                OgreRenderingModule::LogWarning("Mesh " + mesh_name + " has a skeleton with 0 bones. Disabling the skeleton.");
                mesh->setSkeletonName("");
            }
        }
        
        return mesh.get();
    }
}