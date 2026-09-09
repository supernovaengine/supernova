// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Project.h"
#include "imgui.h"
#include "yaml-cpp/yaml.h"

#include <unordered_set>

namespace doriax::editor{

    class AnimationWindow{
    private:

        Project* project;

        // Playback state
        bool isPlaying;
        float currentTime;
        float playbackSpeed;

        // Timeline view state
        float pixelsPerSecond;
        float scrollX;
        float scrollY;
        float minPixelsPerSecond;
        float maxPixelsPerSecond;
        // Manual scrollbar input during playback temporarily takes precedence
        // over automatic playhead following until playback is started again.
        bool playbackFollowSuspended;

        // Selection
        int selectedFrameIndex;
        int prePlaySelectedFrameIndex;
        bool isDraggingPlayhead;
        bool isDraggingFrame;
        int draggingFrameIndex;
        float dragStartTime;
        uint32_t dragStartTrack;

        // Keyframe marker selection and horizontal drag preview
        int selectedKeyFrameIndex;
        int selectedKeyIndex;
        bool isDraggingKey;
        float keyDragStartTime;
        float keyDragTime;

        // Resize
        bool isResizingFrame;
        int resizingFrameIndex;
        int resizeSide; // -1 = left edge, 1 = right edge
        float resizeStartTime;
        float resizeStartDuration;

        // Snap
        bool snapToGrid;
        float snapInterval;

        // Transition preview: currently active top-level animations blended together,
        // the target clip chosen for the next crossfade, and the clip whose time drives
        // the playhead.
        std::vector<Entity> previewAnimations;
        Entity transitionTarget;
        Entity previewPrimary;

        // Cached entity
        Entity selectedEntity;
        uint32_t selectedSceneId;

        // Tab notification
        bool hasNotification;
        bool windowOpen;
        bool focusRequested;
        bool isWindowVisible;

        // Animation preview
        bool isPreviewing;
        struct PreviewEntityState {
            Entity entity = NULL_ENTITY;
            Entity parent = NULL_ENTITY;
            YAML::Node components;
        };
        std::vector<PreviewEntityState> previewState;

        // Keying: right-click a track block or the empty tracks area to store the
        // target's current position/rotation/scale as a keyframe at the clicked
        // time; the toolbar Snapshot button keys every track at the playhead.
        int keyPopupFrameIndex;  // -1 = empty-area popup keying the animation's target
        float keyPopupTime;
        // transient toolbar warning (e.g. key skipped), cleared after a few seconds
        std::string keyNotice;
        double keyNoticeAt;

        // channel: 0 = position, 1 = rotation, 2 = scale
        void keyTargetChannels(Scene* scene, SceneProject* sceneProject, AnimationComponent& anim, Entity target,
                               float time, const std::vector<int>& channels);
        void keyTargetChannel(Scene* scene, SceneProject* sceneProject, AnimationComponent& anim, Entity target, float time, int channel);
        void keyTrackChannel(Scene* scene, SceneProject* sceneProject, Entity trackEntity,
                             float localTime, float explicitDuration, int channel);
        bool hasSnapshotTracks(Scene* scene, const AnimationComponent& anim) const;
        void snapshotTracks(Scene* scene, SceneProject* sceneProject, AnimationComponent& anim);
        // Commit any active key/frame drag and clear its transient state. Playback
        // and preview transitions call this before changing authoring permissions.
        void finishTimelineDrag(Scene* scene, SceneProject* sceneProject, bool refreshPreview);

        // Helpers
        void drawToolbar(float width, AnimationComponent& anim, Scene* scene, SceneProject* sceneProject);
        void drawTimeRuler(ImVec2 canvasPos, ImVec2 canvasSize, float timeStart, float timeEnd);
        bool drawTracks(ImVec2 canvasPos, ImVec2 canvasSize, float timeStart, float timeEnd,
                AnimationComponent& anim, SceneProject* sceneProject, bool allowSelection);
        bool drawPlayhead(ImVec2 canvasPos, ImVec2 canvasSize, float timeStart, float timeEnd);

        float snapTime(float time) const;
        float timeToX(float time, float timeStart, ImVec2 canvasPos) const;
        float xToTime(float x, float timeStart, ImVec2 canvasPos) const;

        std::string getActionLabel(Entity actionEntity, Scene* scene) const;
        std::string getAnimationEntityLabel(Entity entity, Scene* scene) const;
        bool canPreviewEntity(Entity entity, Scene* scene) const;
        void collectPreviewEntitiesRecursive(Scene* scene, Entity entity, std::vector<Entity>& entities,
                             std::unordered_set<Entity>& visitedAnimations,
                             std::unordered_set<Entity>& collectedEntities) const;
        PreviewEntityState buildPreviewEntityState(Scene* scene, Entity entity) const;
        void restorePreviewState(Scene* scene) const;
        void applyPreviewModelBindPose(Scene* scene) const;
        float getAnimationDuration(const AnimationComponent& anim, Scene* scene) const;
        // Frame duration <= 0 means auto: resolved from the action's own duration.
        float effectiveFrameDuration(const ActionFrame& frame, Scene* scene) const;
        // Re-lane frames when overlaps are found on one track (engine-created frames
        // all default to track 0). Marks the scene modified if anything moved.
        void autoAssignTracks(AnimationComponent& anim, SceneProject* sceneProject) const;
        // Seek clipEntity (NULL_ENTITY = the current timeline clip) to `time`,
        // collapsing any active transition preview to that single clip.
        void seekPreview(Scene* scene, SceneProject* sceneProject, float time, Entity clipEntity = NULL_ENTITY);
        // Clip the timeline displays: the blend target while a transition preview is
        // active, otherwise selectedEntity.
        Entity timelineClip(Scene* scene) const;

        void startPreview(Scene* scene, SceneProject* sceneProject);
        void stopPreview(Scene* scene, SceneProject* sceneProject, bool applyBindPose = false);

        // Snapshot + reset one animation's action tree so it can be previewed (and later
        // restored). Used for both the initial clip and any clip a transition blends in.
        void addAnimationToPreview(Scene* scene, Entity animEntity);
        // Crossfade the running preview clip(s) into transitionTarget over its fade time.
        void triggerTransitionPreview(Scene* scene, SceneProject* sceneProject);

    public:
        static constexpr const char* WINDOW_NAME = "Animation";

        AnimationWindow(Project* project);

        void show();
        void setOpen(bool open);
        bool isOpen() const;

        void selectEntity(Entity entity, uint32_t sceneId);

        bool isPreviewingEntity(Entity entity, uint32_t sceneId) const;
        bool getIsPlaying() const;
        bool getIsPreviewing() const;
        float getCurrentTime() const;

        void externalPlay(Entity entity, uint32_t sceneId);
        void externalStop();
        void externalPause();
        void seekPreviewExternal(Scene* scene, SceneProject* sceneProject, float time);
    };

}

