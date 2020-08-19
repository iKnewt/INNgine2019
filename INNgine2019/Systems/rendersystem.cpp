#include "rendersystem.h"
#include "Managers/assetmanager.h"

RenderSystem::RenderSystem()
{

}

void RenderSystem::UpdateLODlevel(std::shared_ptr<MeshComponent> meshComponent, std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<Camera> camera)
{
    if(useLOD_)
    {
        meshComponent->lodLevel_ = 0;
        float distanceToCamera = gsl::Vector3D(transformComponent->position_world_ - camera->position_).length();

        if(AssetManager::GetInstance()->meshManager_->meshes_[meshComponent->meshID_]->VAO_[1] != 0)   //if this mesh has LOD levels
        {
            if (distanceToCamera > distanceLOD1_ && distanceToCamera < distanceLOD2_) //LOD 1
                meshComponent->lodLevel_ = 1;
            else if (distanceToCamera >= distanceLOD2_) //LOD 2
                meshComponent->lodLevel_ = 2;
        }
    }
}

void RenderSystem::SetRenderStyle(RenderStyle renderStyle)
{
    initializeOpenGLFunctions();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    switch (renderStyle)
    {
    case LIT:
        AssetManager::GetInstance()->materialManager_->ResetToDefaults();
        break;
    case UNLIT:
        AssetManager::GetInstance()->materialManager_->ResetToDefaults();
        AssetManager::GetInstance()->materialManager_->SetShaderForAllMaterials(TEXTURE_SHADER);
        break;
    case WIREFRAME:
        AssetManager::GetInstance()->materialManager_->ResetToDefaults();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        break;
    case NORMALS:
        AssetManager::GetInstance()->materialManager_->ResetToDefaults();
        AssetManager::GetInstance()->materialManager_->SetShaderForAllMaterials(PLAIN_SHADER);
        break;
    case UV_MAPS:
        AssetManager::GetInstance()->materialManager_->ResetToDefaults();
        AssetManager::GetInstance()->materialManager_->SetColorForAllMaterials({255});
        AssetManager::GetInstance()->materialManager_->SetTextureForAllMaterials(8);
        AssetManager::GetInstance()->materialManager_->SetShaderForAllMaterials(TEXTURE_SHADER);
        break;
    case LIGHT_ONLY:
        AssetManager::GetInstance()->materialManager_->ResetToDefaults();
        AssetManager::GetInstance()->materialManager_->SetColorForAllMaterials({255});
        AssetManager::GetInstance()->materialManager_->SetTextureForAllMaterials(1);
        AssetManager::GetInstance()->materialManager_->SetShaderForAllMaterials(PHONG_SHADER);
        break;
    }
}

void RenderSystem::Update(std::vector<std::shared_ptr<Camera>> cameras,
                          size_t activeCameraID,
                          size_t activeEntityID,
                          std::shared_ptr<EntityManager> entityManager,
                          std::vector<std::shared_ptr<MeshComponent> > meshComponents,
                          std::vector<std::shared_ptr<TransformComponent> > transformComponents,
                          std::vector<std::shared_ptr<LightComponent>> lightComponents,
                          std::shared_ptr<ShaderManager> shaderManager)
{

    initializeOpenGLFunctions();

    verticesDrawn_ = 0;
    entitiesDrawn_ = 0;


    // RENDER CAMERAS
    if (activeCameraID == 0) // if editor camera is active
    {
        for(size_t i = 1; i < cameras.size(); i++)
        {
            RenderCamera(cameras[i], shaderManager);
        }
    }

    RenderLandscape();

    bool renderActiveEntity = false;

    for (size_t i = 0; i < meshComponents.size(); i++)
    {
        if (!transformComponents[i])
            continue;

        // ADD LIGHT DATA TO PHONG SHADER
        if(lightComponents[i])
        {
            shaderManager->TransmitUniformLightDataToShader(PHONG_SHADER, transformComponents[i]->position_world_, lightComponents[i]);
        }

        if (!meshComponents[i])
            continue;

        // RENDER BOUNDING BOX
        if(showBoundingBoxes_ && AssetManager::GetInstance()->meshManager_->meshes_[meshComponents[i]->meshID_]->boundingBox_)
            RenderOBB(AssetManager::GetInstance()->meshManager_->meshes_[meshComponents[i]->meshID_]->boundingBox_, transformComponents[i], shaderManager);

        if (entityManager->entities_[static_cast<int>(i)]->checkState(0) == Qt::CheckState::Unchecked)
            continue;

        // CHECK IF WITHIN FRUSTUM
        if(useFrustumCulling_)
            if(meshComponents[i]->reactsToFrustumCulling_ && !insideFrustum(cameras[activeCameraID], transformComponents[i]->position_world_, frustumCullingDistance_))
                continue;

        UpdateLODlevel(meshComponents[i], transformComponents[i], cameras[activeCameraID]);

        verticesDrawn_ += AssetManager::GetInstance()->meshManager_->meshes_[meshComponents[i]->meshID_]->numberOfVertices_[meshComponents[i]->lodLevel_];
        entitiesDrawn_++;

        if(i == activeEntityID
                && showSelection_)
            renderActiveEntity = true;
        else
            RenderNormally(meshComponents[i], transformComponents[i], shaderManager);

    }

    if (renderActiveEntity)
        RenderOutline(meshComponents[activeEntityID], transformComponents[activeEntityID], shaderManager);


}

void RenderSystem::RenderCamera(std::shared_ptr<Camera> camera,
                                std::shared_ptr<ShaderManager> shaderManager)
{
    if (!AssetManager::GetInstance()->meshManager_->cameraMesh_)
        return;

    // Camera mesh
    glBindVertexArray( AssetManager::GetInstance()->meshManager_->cameraMesh_->VAO_[0] );
    shaderManager->TransmitUniformDataToShader(AssetManager::GetInstance()->materialManager_->materials_[0], &camera->CameraTransform_->transform_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(AssetManager::GetInstance()->meshManager_->cameraMesh_->numberOfIndices_[0]), GL_UNSIGNED_INT, nullptr);

    //Frustum mesh
    glBindVertexArray(AssetManager::GetInstance()->meshManager_->meshes_[0]->boundingBox_->VAO_);
    shaderManager->TransmitUniformDataToShader(MONO_COLOR_SHADER, &camera->FrustumTransform_->transform_, frustumColor_);
    glDrawElements(GL_LINES, static_cast<GLsizei>(AssetManager::GetInstance()->meshManager_->meshes_[0]->boundingBox_->indices_.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void RenderSystem::RenderLandscape()
{
    if (!AssetManager::GetInstance()->meshManager_->landscapeMesh_)
        return;

    gsl::Matrix4x4 tempModelMatrix;
    tempModelMatrix.setToIdentity();
    // Landscape mesh
    glBindVertexArray( AssetManager::GetInstance()->meshManager_->landscapeMesh_->VAO_[0] );

    AssetManager::GetInstance()->shaderManager_->TransmitUniformDataToShader(AssetManager::GetInstance()->materialManager_->materials_[AssetManager::GetInstance()->landscape_->materialID_], &tempModelMatrix);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(AssetManager::GetInstance()->meshManager_->landscapeMesh_->numberOfIndices_[0]), GL_UNSIGNED_INT, nullptr);
}

void RenderSystem::RenderOBB(std::shared_ptr<BoundingBox> boundingBox,
                             std::shared_ptr<TransformComponent> transformComponent,
                             std::shared_ptr<ShaderManager> shaderManager)
{
    glStencilMask(0x00);

    glBindVertexArray( boundingBox->VAO_);
    shaderManager->TransmitUniformDataToShader(MONO_COLOR_SHADER, &transformComponent->transform_, boundingBoxColor_);
    glDrawElements(GL_LINES, static_cast<GLsizei>(boundingBox->indices_.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void RenderSystem::RenderNormally(std::shared_ptr<MeshComponent> meshComponent,
                                  std::shared_ptr<TransformComponent> transformComponent,
                                  std::shared_ptr<ShaderManager> shaderManager)
{
    glStencilMask(0x00);

    glBindVertexArray( AssetManager::GetInstance()->meshManager_->meshes_[meshComponent->meshID_]->VAO_[meshComponent->lodLevel_] );
    shaderManager->TransmitUniformDataToShader(AssetManager::GetInstance()->materialManager_->materials_[meshComponent->materialID_], &transformComponent->transform_);
    RenderMesh(AssetManager::GetInstance()->meshManager_->meshes_[meshComponent->meshID_], meshComponent->lodLevel_, meshComponent->mode_);
}

void RenderSystem::RenderOutline(std::shared_ptr<MeshComponent> meshComponent,
                                 std::shared_ptr<TransformComponent> transformComponent,
                                 std::shared_ptr<ShaderManager> shaderManager)
{
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should update the stencil buffer
    glStencilMask(0xFF); // enable writing to the stencil buffer

    // RENDER OBJECT
    glBindVertexArray( AssetManager::GetInstance()->meshManager_->meshes_[meshComponent->meshID_]->VAO_[meshComponent->lodLevel_] );
    shaderManager->TransmitUniformDataToShader(AssetManager::GetInstance()->materialManager_->materials_[meshComponent->materialID_], &transformComponent->transform_);
    RenderMesh(AssetManager::GetInstance()->meshManager_->meshes_[meshComponent->meshID_], meshComponent->lodLevel_, meshComponent->mode_);

    // RENDER OUTLINE
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); // disable writing to the stencil buffer
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray( AssetManager::GetInstance()->meshManager_->meshes_[meshComponent->meshID_]->VAO_[meshComponent->lodLevel_] );

    gsl::Matrix4x4 tempTransform = transformComponent->transform_;
    tempTransform.scale(calculateOutlineSize(transformComponent->scale_relative_));

    shaderManager->TransmitUniformDataToShader(MONO_COLOR_SHADER, &tempTransform, selectionColor_);
    RenderMesh(AssetManager::GetInstance()->meshManager_->meshes_[meshComponent->meshID_], meshComponent->lodLevel_, GL_TRIANGLES);

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
}

gsl::Vector3D RenderSystem::calculateOutlineSize(gsl::Vector3D originalSize)
{
    gsl::Vector3D temp;

    temp.x = -0.17f * originalSize.x + 1.2f;
    temp.y = -0.17f * originalSize.y + 1.2f;
    temp.z = -0.17f * originalSize.z + 1.2f;

    if(temp.x < 1.01f)
        temp.x = 1.01f;
    if(temp.y < 1.01f)
        temp.y = 1.01f;
    if(temp.z < 1.01f)
        temp.z = 1.01f;

    return temp;
}

void RenderSystem::RenderMesh(std::shared_ptr<Mesh> mesh, uint lodLevel, GLenum mode)
{
    if (mesh->numberOfIndices_[lodLevel] > 0)
        glDrawElements(mode, static_cast<GLsizei>(mesh->numberOfIndices_[lodLevel]), GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(mode, 0, static_cast<GLsizei>(mesh->numberOfVertices_[lodLevel]));
    glBindVertexArray(0);
}

bool RenderSystem::insideFrustum(std::shared_ptr<Camera> camera, gsl::Vector3D position, float radius)
{
    for (unsigned int i{0}; i < 6; i++)
    {
        gsl::Vector3D normal = camera->frustum_[i].normal;
        float distance = camera->frustum_[i].distanceToOrigo;
        float distanceToPlane = gsl::Vector3D::dot(position, normal) + distance;

        if(distanceToPlane < radius)    //This should be distanceToPlane < 0.f and radius tested another way
            return false;
    }
    return true;
}
