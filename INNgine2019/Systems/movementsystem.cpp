#include "movementsystem.h"
#include "GSL/gsl_math.h"

#include <quaternion.h>

void MovementSystem::Update(std::shared_ptr<EntityManager> entityManager, std::vector<std::shared_ptr<TransformComponent> > transformComponents, std::vector<std::shared_ptr<MeshComponent> > meshComponents, std::vector<std::shared_ptr<AIComponent> > AIComponents , std::shared_ptr<Landscape> landscape_, std::vector<std::shared_ptr<LightComponent> > lightComponents, size_t PlayerID)
{
    UpdateMovement(transformComponents,meshComponents,AIComponents,landscape_);

    for(size_t i = 0; i< caughtTrophies_.size(); i++)
        UpdateTrophyAnimation(transformComponents[caughtTrophies_[i]],transformComponents[PlayerID],i);

    for (size_t i = 0; i < transformComponents.size(); i++)
    {
        if (!transformComponents[i])
            continue;
        if(AIComponents[i] && update_)
        {
            if(transformComponents[PlayerID])
                UpdateAIMovement(transformComponents[i],AIComponents[i], i, landscape_,transformComponents[PlayerID]->position_world_);
            else
                UpdateAIMovement(transformComponents[i],AIComponents[i], i, landscape_,{0,0,0});
        }

        // update modelMatrix
        UpdateTransform(entityManager,transformComponents,i);

        if(lightComponents[i])
            if(lightComponents[i]->useEntityTransformForwardVectorAsDirection_)
                lightComponents[i]->direction_ = transformComponents[i]->transform_.GetForwardVector().normalized();
    }
}

std::vector<gsl::Vector3D> MovementSystem::UpdateBoundingBoxes(size_t meshID, gsl::Vector3D position, std::shared_ptr<TransformComponent> transformComponent)
{
    std::vector<gsl::Vector3D> updatedBox;
    std::shared_ptr<TransformComponent> x = std::make_shared<TransformComponent>();
    *x = *transformComponent;
    x->position_relative_ = (x->position_relative_ + (position));
    for(gsl::Vector3D point : AssetManager::GetInstance()->meshManager_->meshes_[meshID]->boundingBox_->points_)
    {
        gsl::Vector4D vec = x->transform_ * gsl::Vector4D(point+ (position));
        //        meshComponent->mesh_->boundingBox_->pointsTransformed_.push_back(point + position + transformComponent->position_world_);
        updatedBox.push_back(vec.toVector3D());
    }
    return updatedBox;

}

void MovementSystem::UpdateMovement(std::vector<std::shared_ptr<TransformComponent> > transformComponents, std::vector<std::shared_ptr<MeshComponent> > meshComponent, std::vector<std::shared_ptr<AIComponent>> AIComponents,std::shared_ptr<Landscape> landscape_)
{
    for(size_t i = 0; i < movements_.size(); i++)
    {
        if(!isColliding(movements_[i].first, meshComponent, transformComponents, movements_[i].second))
        {
            transformComponents[movements_[i].first]->position_relative_ += movements_[i].second;
            gsl::Vector3D entityPosition = transformComponents[movements_[i].first]->position_relative_;
            if(transformComponents[movements_[i].first]->followLandscape_ && !AIComponents[movements_[i].first])
            {
                transformComponents[movements_[i].first]->position_relative_ = {entityPosition.x,FindLandscapeYOnLocation(landscape_,entityPosition),entityPosition.z};
            }
            if(transformComponents[movements_[i].first]->orientRotationBasedOnMovement_ && !AIComponents[movements_[i].first])
            {
                transformComponents[movements_[i].first]->newForwardVector_ = movements_[i].second.normalized();
            }
        }
    }
    movements_.clear();
}

bool MovementSystem::isColliding(size_t ID, std::vector<std::shared_ptr<MeshComponent> > meshComponents,std::vector<std::shared_ptr<TransformComponent> > transformComponents, gsl::Vector3D position)
{
    if(!meshComponents[ID])
        return false;
    std::vector<gsl::Vector3D> mainUpdatedPoints = UpdateBoundingBoxes(meshComponents[ID]->meshID_, position, transformComponents[ID]);
    float x = mainUpdatedPoints[1].x - mainUpdatedPoints[2].x;
    float z = mainUpdatedPoints[0].z - mainUpdatedPoints[1].z;
    std::vector<gsl::Vector3D> mainLineX = gsl::ProjectPointsOnVector(mainUpdatedPoints, gsl::Vector3D(x,0,0).normalized());
    std::vector<gsl::Vector3D> mainLineY = gsl::ProjectPointsOnVector(mainUpdatedPoints, gsl::Vector3D(0,1,0));
    std::vector<gsl::Vector3D> mainLineZ = gsl::ProjectPointsOnVector(mainUpdatedPoints, gsl::Vector3D(0,0,z).normalized());
    std::pair<float, float> mainMinMaxX = gsl::findMinAndMaxInX(mainLineX);
    std::pair<float, float> mainMinMaxY = gsl::findMinAndMaxInY(mainLineY);
    std::pair<float, float> mainMinMaxZ = gsl::findMinAndMaxInZ(mainLineZ);

    for(size_t otherID = 0; otherID < meshComponents.size(); otherID++)
    {

        if(otherID == ID)
            continue;
        if(!meshComponents[otherID])
            continue;
        if(!meshComponents[otherID]->enableCollision_)
            continue;

        std::vector<gsl::Vector3D> iUpdatedPoints = UpdateBoundingBoxes(meshComponents[otherID]->meshID_, gsl::Vector3D(0,0,0), transformComponents[otherID]);
        float x = iUpdatedPoints[1].x - iUpdatedPoints[2].x;
        float z = iUpdatedPoints[0].z - iUpdatedPoints[1].z;
        std::vector<gsl::Vector3D> iLineX = gsl::ProjectPointsOnVector(iUpdatedPoints, gsl::Vector3D(x,0,0).normalized());
        std::vector<gsl::Vector3D> iLineY = gsl::ProjectPointsOnVector(iUpdatedPoints, gsl::Vector3D(0,1,0));
        std::vector<gsl::Vector3D> iLineZ = gsl::ProjectPointsOnVector(iUpdatedPoints, gsl::Vector3D(0,0,z).normalized());
        std::pair<float, float> iMinMaxX = gsl::findMinAndMaxInX(iLineX);
        std::pair<float, float> iMinMaxY = gsl::findMinAndMaxInY(iLineY);
        std::pair<float, float> iMinMaxZ = gsl::findMinAndMaxInZ(iLineZ);

        if(overlaps(mainMinMaxX.first,mainMinMaxX.second,iMinMaxX.first,iMinMaxX.second))
            if(overlaps(mainMinMaxY.first,mainMinMaxY.second,iMinMaxY.first,iMinMaxY.second))
                if(overlaps(mainMinMaxZ.first,mainMinMaxZ.second,iMinMaxZ.first,iMinMaxZ.second))
                {
                    if(meshComponents[ID]->objectType_ == ENEMY)
                    {
                        if(meshComponents[otherID]->objectType_ == PLAYER)
                        {
                            AssetManager::GetInstance()->AddEvent(0);
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else if(meshComponents[ID]->objectType_ == PLAYER)
                    {
                        if(meshComponents[otherID]->objectType_ == ENEMY)
                        {
                            AssetManager::GetInstance()->AddEvent(0);
                        }
                        else if(meshComponents[otherID]->objectType_ == TROPHY)
                        {
                            AssetManager::GetInstance()->AddEvent(static_cast<unsigned int>(otherID));
                            meshComponents[otherID]->enableCollision_ = false;
                        }
                    }
                    return true;
                }
    }

    //No collision found.
    return false;
}

void MovementSystem::AddMovement(size_t ID, gsl::Vector3D value)
{
    movements_.push_back(std::make_pair(ID,value));
    //    qDebug() << ID << " | " << value;
}

float MovementSystem::FindLandscapeYOnLocation(std::shared_ptr<Landscape> landscape_, gsl::Vector3D location)
{
    if(!landscape_->indices_.empty())
    {
        std::vector<GLuint> ID = landscape_->indices_;
        for(unsigned int i = 2; i < ID.size() ; i = i + 3)
        {
            gsl::Vector2D tri1(landscape_->vertices_[ID[i-2]].XYZ_.x,landscape_->vertices_[ID[i-2]].XYZ_.z);
            gsl::Vector2D tri2(landscape_->vertices_[ID[i-1]].XYZ_.x,landscape_->vertices_[ID[i-1]].XYZ_.z);
            gsl::Vector2D tri3(landscape_->vertices_[ID[i]].XYZ_.x,landscape_->vertices_[ID[i]].XYZ_.z);
            gsl::Vector3D newplayerPos = gsl::barycentricCoordinates(tri3,tri2,tri1,gsl::Vector2D(location.x,location.z));
            if(newplayerPos.getX()>0 && newplayerPos.getX()<1 &&newplayerPos.getY()>0 && newplayerPos.getY()<1 && newplayerPos.getZ()>0 && newplayerPos.getZ() <1)
            {
                float playerTempPos = (landscape_->vertices_[ID[i-2]].XYZ_.y *newplayerPos.getZ() +
                        landscape_->vertices_[ID[i-1]].XYZ_.y*newplayerPos.getY() +
                        landscape_->vertices_[ID[i]].XYZ_.y*newplayerPos.getX());
                return playerTempPos;
            }
        }
    }
    return 0;
}

void MovementSystem::UpdateAIMovement(std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<AIComponent> AIComponent, size_t ID, std::shared_ptr<Landscape> landscape_, gsl::Vector3D playerPosition)
{
    gsl::Vector3D movement(0,0,0);
    if(AIComponent->aiState_ == AI_PATROL)
        movement = AIPatrol(transformComponent,AIComponent,landscape_);
    else // AIComponent->aiState_ == AI_CHASE
        movement = AIChase(transformComponent,landscape_,playerPosition);
    AddMovement(ID, movement);
    CheckAIState(AIComponent,transformComponent->position_world_,playerPosition);

}

void MovementSystem::CheckAIState(std::shared_ptr<AIComponent> AIComponent, gsl::Vector3D AIPosition, gsl::Vector3D playerPosition)
{
    if(AIComponent->aiState_ == AI_PATROL)
    {
        if((AIPosition - playerPosition).length() < AIComponent->distanceBeforeChase_)
        {
            AIComponent->aiState_ = AI_CHASE;
        }
    }
    else // AIComponent->aiState_ == AI_CHASE
    {
        if((AIPosition - playerPosition).length() > AIComponent->distanceBeforeChase_)
        {
            AIComponent->aiState_ = AI_PATROL;
            AIComponent->spline_->UpdatePoint(AIPosition);
            AIComponent->locationOnSpline_ = 0;
        }
    }
}

gsl::Vector3D MovementSystem::AIPatrol(std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<AIComponent> AIComponent,std::shared_ptr<Landscape> landscape_)
{
    Lerp(AIComponent, transformComponent, landscape_);
    int knotBehind = AIComponent->spline_->findKnotInterval(AIComponent->locationOnSpline_);
    gsl::Vector3D calculateSpline = AIComponent->spline_->EvaluateBSpline(knotBehind,AIComponent->locationOnSpline_);
    if(transformComponent->followLandscape_)
        calculateSpline.y = FindLandscapeYOnLocation(landscape_,transformComponent->position_relative_);
    gsl::Vector3D movement = calculateSpline - transformComponent->position_relative_;
    if(transformComponent->orientRotationBasedOnMovement_)
    {
        transformComponent->newForwardVector_ =  movement;
        //setting y as 0 because we only want it to rotate on the XZ basis.
        transformComponent->newForwardVector_.y = 0;
        //For some reason, the lookAt function won't work properly if i dont multiply z with -1.
        transformComponent->newForwardVector_.z *= -1.f;
        transformComponent->newForwardVector_.normalize();
    }
    return movement;
}

gsl::Vector3D MovementSystem::AIChase(std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<Landscape> landscape_,gsl::Vector3D playerPosition)
{
    //    gsl::Vector3D playerpos(playerPosition.x,playerpositio,playerPosition.z);
    //    gsl::Vector3D AIpos(transformComponent->position_relative_.x,0,transformComponent->position_relative_.z);
    //    gsl::Vector3D towardsPlayer = playerpos - AIpos;
    gsl::Vector3D towardsPlayer = playerPosition - transformComponent->position_relative_;
    towardsPlayer.normalize();
    towardsPlayer = (towardsPlayer * 0.0125f) * AssetManager::GetInstance()->deltaTime_;
    if(transformComponent->followLandscape_)
        towardsPlayer.y = FindLandscapeYOnLocation(landscape_,transformComponent->position_relative_ + towardsPlayer) - transformComponent->position_relative_.y;

    if(transformComponent->orientRotationBasedOnMovement_)
    {
        transformComponent->newForwardVector_ =  towardsPlayer;
        //setting y as 0 because we only want it to rotate on the XZ basis.
        transformComponent->newForwardVector_.y = 0;
        //For some reason, the lookAt function won't work properly if i dont multiply z with -1.
        transformComponent->newForwardVector_.z *= -1.f;
        transformComponent->newForwardVector_.normalize();
    }
    return towardsPlayer;
}

void MovementSystem::Lerp(std::shared_ptr<AIComponent> AIComponent, std::shared_ptr<TransformComponent> transformComponent, std::shared_ptr<Landscape> landscape_)
{
    if(AIComponent->locationOnSpline_ > 1)
    {
        AIComponent->spline_->RemoveControlPoints();
        AIComponent->spline_->RandomizeControlpointOrder();
        AIComponent->spline_->SetStartToEnd();
        AIComponent->locationOnSpline_ = 0;
        gsl::Vector3D AILocation = AIComponent->spline_->EvaluateBSpline(AIComponent->spline_->findKnotInterval(0),0);
        AILocation.y = FindLandscapeYOnLocation(landscape_,AILocation);
        transformComponent->position_relative_ = AILocation;
    }
    else
        AIComponent->locationOnSpline_ += (AIComponent->speed_ * 5.f/ AIComponent->spline_->controlPoints_.size()) * AssetManager::GetInstance()->deltaTime_;
}

void MovementSystem::UpdateTrophyAnimation(std::shared_ptr<TransformComponent> cowTransform, std::shared_ptr<TransformComponent> playerTransform, size_t index)
{
    //Y height maximum == 5
    if(cowTransform->position_world_.y > playerTransform->position_world_.y +4.5f || cowTransform->scale_relative_.x < 0.01f)
    {
        cowTransform->position_relative_ += 50.f;
        caughtTrophies_.erase(caughtTrophies_.begin()+static_cast<unsigned int>(index));
    }
    cowTransform->rotation_relative_.y += 1.f * AssetManager::GetInstance()->deltaTime_;
    cowTransform->scale_relative_ -= (cowTransform->scale_relative_ * gsl::Vector3D(0.01f,-0.001f,0.01f) * AssetManager::GetInstance()->deltaTime_);
    gsl::Vector3D cowToPlayer = (playerTransform->position_world_ + gsl::Vector3D(0,5,0)) - cowTransform->position_world_;
    cowToPlayer.x *= 15;
    cowToPlayer.z *= 15;
    cowToPlayer.normalize();
    cowToPlayer = (cowToPlayer / (40.f)) * AssetManager::GetInstance()->deltaTime_;
    cowTransform->position_relative_ += cowToPlayer ;

}

void MovementSystem::UpdateTransform(std::shared_ptr<EntityManager> entityManager, std::vector<std::shared_ptr<TransformComponent> > transformComponents, size_t ID)
{
    size_t parentID = entityManager->GetParentEntityID(ID);
    if (parentID != gsl::INVALID_SIZE)
        UpdateFromParent(transformComponents[ID],transformComponents[static_cast<unsigned long long>(parentID)]);
    else
        UpdateTransformMatrix(transformComponents[ID]);
}

void MovementSystem::UpdateFromParent(std::shared_ptr<TransformComponent> transform, std::shared_ptr<TransformComponent> transformParent)
{

    transform->position_world_ = transform->position_relative_+transformParent->position_world_;
    transform->rotation_world_ = transform->rotation_relative_+transformParent->rotation_world_;
    transform->scale_world_ = transform->scale_relative_*transformParent->scale_world_;

    //Moves back to proper position
    gsl::Matrix4x4 positionMatrix_;
    positionMatrix_.setToIdentity();
    positionMatrix_.translate(transform->position_relative_);
    gsl::Matrix4x4 rotationMatrix_;
    rotationMatrix_.setToIdentity();
    rotationMatrix_.rotateX(transform->rotation_relative_.x);
    rotationMatrix_.rotateY(transform->rotation_relative_.y);
    rotationMatrix_.rotateZ(transform->rotation_relative_.z);
    gsl::Matrix4x4 scaleMatrix_;
    scaleMatrix_.setToIdentity();
    gsl::Vector3D addedScale{transform->scale_relative_.x,transform->scale_relative_.y,transform->scale_relative_.z};
    scaleMatrix_.scale(addedScale);

    //Creates the final transform of the child
    transform->transform_ = transformParent->transform_ * (positionMatrix_ * rotationMatrix_ * scaleMatrix_);
}

void MovementSystem::UpdateTransformMatrix(std::shared_ptr<TransformComponent> transform)
{

    transform->position_world_ = transform->position_relative_;
    transform->rotation_world_ = transform->rotation_relative_;
    transform->scale_world_ = transform->scale_relative_;

    gsl::Matrix4x4 positionMatrix_;
    positionMatrix_.setToIdentity();
    positionMatrix_.translate(transform->position_relative_);

    gsl::Matrix4x4 rotationMatrix_;
    rotationMatrix_.setToIdentity();
    if(transform->orientRotationBasedOnMovement_)
    {
        rotationMatrix_.lookAt({0,0,0}, transform->newForwardVector_,{0,1,0});
        rotationMatrix_.setPosition(0,0,0);
    }
    else
    {
        gsl::Quaternion finalRotation(0,0,0,0);
        gsl::Quaternion xRotation = finalRotation.Rotate(transform->rotation_relative_.x,gsl::Vector3D(1,0,0));
        gsl::Quaternion yRotation = finalRotation.Rotate(transform->rotation_relative_.y,gsl::Vector3D(0,1,0));
        gsl::Quaternion zRotation = finalRotation.Rotate(transform->rotation_relative_.z,gsl::Vector3D(0,0,1));
        finalRotation = zRotation * yRotation *xRotation;
        rotationMatrix_.rotQuat(finalRotation.x,finalRotation.y,finalRotation.z,finalRotation.w);
    }
    gsl::Matrix4x4 scaleMatrix_;
    scaleMatrix_.setToIdentity();
    gsl::Vector3D addedScale{transform->scale_relative_.x,transform->scale_relative_.y,transform->scale_relative_.z};
    scaleMatrix_.scale(addedScale);
    transform->transform_ = positionMatrix_ * rotationMatrix_ * scaleMatrix_;
}
