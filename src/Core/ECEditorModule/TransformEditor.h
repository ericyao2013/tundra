/**
    For conditions of distribution and use, see copyright notice in LICENSE

    @file   TransformEditor.h
    @brief  Controls Transform attributes for group of entities. */

#pragma once

#include "ECEditorModuleApi.h"
#include "ui_EditorSettings.h"

#include "SceneFwd.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "InputFwd.h"
#include "IAttribute.h"

#ifdef EC_TransformGizmo_ENABLED
#include "EC_TransformGizmo.h"
#endif

#include <QPointer>

class OgreWorld;
class UndoManager;
#ifdef EC_TransformGizmo_ENABLED
class EC_TransformGizmo;
#endif

class EditorSettings : public QWidget, public Ui::EditorSettings
{
    Q_OBJECT

public:
    explicit EditorSettings(QWidget *parent) : QWidget(parent) { setupUi(this); }
};

/// Controls Transform attributes for group of entities.
/** Can be used to alter transforms of entities even without the visual gizmo (EC_TransformGizmo).*/
class ECEDITOR_MODULE_API TransformEditor : public QObject
{
    Q_OBJECT

public:
    /// Constructs the editor.
    /** Creates EC_TransformGizmo if it is available.
        @param editedScene Scene in which the edited entities reside.
        @param undoManager If you wish to perform undo and redo operations, pass in your UndoManager instance. */
    explicit TransformEditor(const ScenePtr &editedScene, UndoManager *undoManager = 0);

    /// Destroys the editor.
    /** Destroys the EC_TransformGizmo if it was created. */
    ~TransformEditor();
    
    /// Enable or disable user interface windows.
    /** This widget will by default create a settings window.
        @note This needs to be called before settings the selection
        or the window can be seen on screen for a split second. */
    void SetUserInterfaceEnabled(bool enabled);
    
    //// Enable or disable placeable axis visualization.
    /** Enabled by default. */
    void SetDragPlaceableAxisEnabled(bool enabled);

    /// Returns the current selection
    EntityList Selection() const;

    /// Sets new selection of entities, clears possible previous selection.
    /** @param entities Entities to be added. */
    void SetSelection(const EntityList &entities);
    void SetSelection(const EntityPtr &entity); /**< @overload */

    /// Appends selection with new entities.
    /** @param entities Entities to be added. */
    void AppendSelection(const EntityList &entities);
    void AppendSelection(const EntityPtr &entity); /**< @overload */

    /// Removes entities from selection.
    /** @param entities Entities to be removed. */
    void RemoveFromSelection(const EntityList &entities);
    void RemoveFromSelection(const EntityPtr &entity); /**< @overload */

    /// Clears the selection.
    void ClearSelection();

    /// Focuses the transform of gizmo (if used) to center of AABB
    void FocusGizmoPivotToAabbCenter();

    /// Sets visibility of the gizmo (if used).
    void SetGizmoVisible(bool show);

    /// Set gizmo type.
    void SetGizmoUsesLocalAxes(bool enabled);

    /// Returns position of the editing gizmo.
    float3 GizmoPos() const;

#ifdef EC_TransformGizmo_ENABLED
    /// Returns the transform gizmo.
    EC_TransformGizmo *TransformGizmo() const;
    
    /// Set gizmo type.
    /** @note You must set selection first so that the gizmo is created. */
    void SetGizmoType(EC_TransformGizmo::GizmoType type);
#endif

    /// Returns the transform gizmo editor settings widget.
    QPointer<EditorSettings> EditorSettingsWidget() const { return editorSettings; }

    /// Translates a transform attribute.
    static void Translate(const TransformAttributeWeakPtr &attr, const float3 &offset, AttributeChange::Type change);

    /// Rotates a transform attribute.
    static void Rotate(const TransformAttributeWeakPtr &attr, const float3x4 &rotation, AttributeChange::Type change);

    /// Scales a transform attribute.
    static void Scale(const TransformAttributeWeakPtr &attr, const float3 &offset, AttributeChange::Type change);

public slots:
    /// Translates current target transforms.
    /** @param offset Offset to be applied. */
    void TranslateTargets(const float3 &offset);

    /// Rotates current target transforms.
    /** @param delta Change to be applied. */
    void RotateTargets(const Quat &delta);

    /// Scales current target transforms.
    /** @param offset Offset to be applied. */
    void ScaleTargets(const float3 &offset);

private:
    /// Returns whether or not transform attribute @attr is parented and current selection of targets contain also the parent.
    bool TargetsContainAlsoParent(const TransformAttributeWeakPtr &attr) const;

    /// Creates transform gizmo for the editor.
    void CreateGizmo();

    /// Destroys editor's transform gizmo.
    void DeleteGizmo();

    /// Draw debug visualization for an entity.
    void DrawDebug(OgreWorld* world, Entity* entity);

    SceneWeakPtr scene; ///< Scene in which the edited entities reside.
    EntityPtr gizmo; ///< Gizmo entity.
    QList<TransformAttributeWeakPtr> targets; ///< Current target transform attributes.
    UndoManager *undoManager; ///< Undo manager, if undo functionality used.
    
    InputContextPtr input; ///< Input context for controlling gizmo mode.
    QPointer<EditorSettings> editorSettings; ///< Editor settings window
    
    bool localAxes; ///< Whether to show object local axes instead of global world axes.
    bool uiAllowed;
    bool debugAxisAllowed;

private slots:
    /// Handles KeyEvents and changes gizmo's mode.
    /** @param e Key event. */
    void HandleKeyEvent(KeyEvent *e);

    /// Handles frame update. Redraws debug visualizations as necessary.
    void OnUpdated(float frameTime);
    
    /// Gizmo mode has been selected from the settings widget.
    void OnGizmoModeSelected(int mode);
    /// Gizmo axis has been selected from the settings widget.
    void OnGizmoAxisSelected(int axis);
};
