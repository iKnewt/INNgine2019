#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <QOpenGLFunctions_4_1_Core>
#include "Managers/entitymanager.h"
#include "Managers/meshmanager.h"
#include "Managers/shadermanager.h"
#include "Legacy/camera.h"

/// Used to change all materials to match a certain render style
/// Should be a part of the render system, but for lack of time is placed here
enum RenderStyle
{
    LIT,
    UNLIT,
    WIREFRAME,
    NORMALS,
    UV_MAPS,
    LIGHT_ONLY
};

/// Contains the logic on how to render objects every tick.
class RenderSystem : public QOpenGLFunctions_4_1_Core
{
public:
    RenderSystem();

    /// Number of vertices drawn each frame, used to see perfomance.
    size_t verticesDrawn_{0};
    /// Number of entities drawn each frame, used to see perfomance.
    size_t entitiesDrawn_{0};

    /// Whether to use frustum culling.
    bool useFrustumCulling_{true};
    /// Whether to render bounding boxes.
    bool showBoundingBoxes_{false};
    /// Whether to render frustum for each camera.
    bool showFrustum_{false};
    /// Whether to use different LOD when rendering mesh.
    bool useLOD_{true};
    /// Whether to render outline for selected entity.
    bool showSelection_{true};
    /// Defines distance between camera and entity befor using a level 1 LOD for the entity.
    float distanceLOD1_{20};
    /// Defines distance between camera and entity befor using level 2 LOD for the entity.
    float distanceLOD2_{50};
    /// Defines distance from entity origo frustum plane can be before entity is not rendered.
    float frustumCullingDistance_{-0.8f};
    /// Color of Boundting Boxes when rendered.
    gsl::Vector3D boundingBoxColor_{255, 0, 0};
    /// Color of Outline on Selected entity.
    gsl::Vector3D selectionColor_{0, 255, 255};
    /// Color of Outline on Selected entity.
    gsl::Vector3D frustumColor_{255, 200, 0};

    /**
     * Updates the rendersystem, should be called every tick to render to screen.
     * @param cameras All cameras in scene.
     * @param activeCameraID
     * @param activeEntityID
     * @param entityManager
     * @param meshComponents
     * @param transformComponents
     * @param lightComponents
     * @param shaderManager to get acces to all shaders.
     */
    void Update(std::vector<std::shared_ptr<Camera> > cameras, size_t activeCameraID, size_t activeEntityID, std::shared_ptr<EntityManager> entityManager,
                std::vector<std::shared_ptr<MeshComponent>> meshComponents,
                std::vector<std::shared_ptr<TransformComponent>> transformComponents,
                std::vector<std::shared_ptr<LightComponent>> lightComponents,
                std::shared_ptr<ShaderManager> shaderManager);
    /**
     * Renders the scene landscape.
     */
    void RenderLandscape();
    /**
     * Sets the renderstyle for the scene.
     * @param renderStyle The render style to use.
     */
    void SetRenderStyle(RenderStyle renderStyle);
private:
    /**
     * Renders camera and it's frustum.
     * @param camera Camera to be rendered.
     * @param shaderManager
     */
    void RenderCamera(std::shared_ptr<Camera> camera, std::shared_ptr<ShaderManager> shaderManager);
    /**
     * Renders a Bounding box.
     * @param boundingBox Bounding box to render.
     * @param transformComponent Transform to use for bounding box.
     * @param shaderManager
     */
    void RenderOBB(std::shared_ptr<BoundingBox> boundingBox,
                   std::shared_ptr<TransformComponent> transformComponent,
                   std::shared_ptr<ShaderManager> shaderManager);
    /**
     * Renders a mesh.
     * Creates a stencilMask from a mesh when rendering it,
     * @param meshComponent Mesh to render.
     * @param transformComponent Transform to use for mesh.
     * @param shaderManager
     */
    void RenderNormally(std::shared_ptr<MeshComponent> meshComponent,
                      std::shared_ptr<TransformComponent> transformComponent,
                      std::shared_ptr<ShaderManager> shaderManager);
    /**
     * Renders a mesh and an outline for the mesh.
     * Creates a stencilMask from a mesh when rendering it,
     * then renders a slightly larger version of the mesh in a single color shader not overriding the stencilMask.
     * This creates a colored outline for the mesh.
     * These meshes need to be rendered after all other mesh.
     * @param meshComponent Mesh to render.
     * @param transformComponent Transform to use for mesh.
     * @param shaderManager
     */
    void RenderOutline(std::shared_ptr<MeshComponent> meshComponent,
                       std::shared_ptr<TransformComponent> transformComponent,
                       std::shared_ptr<ShaderManager> shaderManager);
    /**
     * Checks if raius around position is within camera frustum.
     * Used for frustum culling to see if mesh should be rendered.
     * @param camera Active Camera.
     * @param position Entity position.
     * @param radius Radius around position to check for.
     * @return Whether entity is within camera frustum.
     */
    bool insideFrustum(std::shared_ptr<Camera> camera, gsl::Vector3D position, float radius);
    /**
     * Calculates a new slightly bigger scale to be used for outline based on mesh' original size.
     * @param originalSize Mesh' original scale.
     * @return New scale to be used for outline.
     */
    gsl::Vector3D calculateOutlineSize(gsl::Vector3D originalSize);
    /**
     * OpenGL draw calls for basic mesh.
     * @param mesh mesh to render.
     * @param lodLevel current lod level to render.
     * @param mode what mode to render in.
     */
    void RenderMesh(std::shared_ptr<Mesh> mesh, uint lodLevel = 0, GLenum mode = GL_TRIANGLES);
    /**
     * Checks what LOD level to use and updates the mesh component
     * @param meshComponent mesh component to update/check.
     * @param transformComponent transform component to check.
     * @param camera Camera to check distance from.
     */
    void UpdateLODlevel(std::shared_ptr<MeshComponent> meshComponent, std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<Camera> camera);


};

#endif // RENDERSYSTEM_H
