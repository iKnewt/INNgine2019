#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "Managers/entitymanager.h"
#include "Managers/assetmanager.h"


//#include "landscape.h"
///Calculates and handles everything movement, and collisions.
class MovementSystem
{
public:
    MovementSystem(){}

    /**
     * Updates the movement for each Entity that moves. Needs to be done each tick.
     * @param entityManager The Entity Manager that keeps all information about parent/child relationships.
     * @param transformComponents The std::Vector of all Transform Components.
     * @param meshComponents The std::Vector of all Mesh Components.
     * @param AIComponents The std::Vector of all AI Components.
     * @param landscape_ The landscape that the entity may follow. used for Barycentric Cordinates.
     */
    void Update(std::shared_ptr<EntityManager> entityManager,
                std::vector<std::shared_ptr<TransformComponent> > transformComponents,
                std::vector<std::shared_ptr<MeshComponent> > meshComponents,
                std::vector<std::shared_ptr<AIComponent> > AIComponents ,
                std::shared_ptr<Landscape> landscape_,
                std::vector<std::shared_ptr<LightComponent> > lightComponents,
                size_t PlayerID = 0);
    /**
     * Goes through each movement input given to the system and uses the isColliding function to check collisions.
     * @param transformComponents The std::Vector of all Transform Components.
     * @param meshComponent The std::Vector of all Mesh Components.
     * @param AIComponentsThe std::Vector of all AI Components.
     * @param landscape_ The landscape that the entity may follow. used for Barycentric Cordinates.
     */
    void UpdateMovement(std::vector<std::shared_ptr<TransformComponent> > transformComponents,
                        std::vector<std::shared_ptr<MeshComponent> > meshComponent,
                        std::vector<std::shared_ptr<AIComponent>> AIComponents,
                        std::shared_ptr<Landscape> landscape_);
    /**
     * Moves the character the specified amount, if no collisions happen.
     * @param ID The Entity ID.
     * @param value Moving amount in specified direction
     */
    void AddMovement(size_t ID, gsl::Vector3D value);
    /**
     * Goes through the specified landscape and finds the Y coordinate of the specified location.
     * Used to set the Entity's height so that it corresponds to the landscape.
     * @param landscape_ The landscape to find the height in.
     * @param location The location of the entity, Usually their position in world.
     * @return the height amount on the landscape's specified location.
     */
    static float FindLandscapeYOnLocation(std::shared_ptr<Landscape> landscape_, gsl::Vector3D location);

    ///Container that has the ID's of the cows that are currently getting caught. Used in the UpdateTrophyAnimation function.
    std::vector<size_t> caughtTrophies_;

    bool update_{false};

    /**
     * Updates the transform of the specified Entity. Use this if there is a chance a Entity has a parent.
     * @param entityManager To check parent/child relationships.
     * @param transformComponents The std::Vector of all Transform Components.
     * @param ID The ID of the Entity.
     */
    void UpdateTransform(std::shared_ptr<EntityManager> entityManager,
                         std::vector<std::shared_ptr<TransformComponent> > transformComponents,
                         size_t ID);
    /**
     * Updates the transform matrix of the specified Transform Component.
     * This function is unsafe, and must only be used if there is no possibility of the Entity having a parent Entity.
     * @param transform the Transform Component that to be updated.
     */
    void UpdateTransformMatrix(std::shared_ptr<TransformComponent> transform);

private:
    /**
     * Updates the specified Mesh Component's bounding box for collision check. is used in the isColliding function.
     * @param meshComponent the Mesh Component to update.
     * @param position the movement the Entity wants to do
     * @param transformComponent the Transform Component to multiply each point with.
     * @return gives the specified mesh component's bounding box.
     */
    std::vector<gsl::Vector3D> UpdateBoundingBoxes(size_t meshComponent,
                                                   gsl::Vector3D position,
                                                   std::shared_ptr<TransformComponent> transformComponent);
    /**
     * Goes through all bounding boxes to make sure no colliding happens with the movement input given.
     * @param ID The entity that moves' ID.
     * @param meshComponents The std::Vector of all Mesh Components.
     * @param transformComponents The std::Vector of all Transform Components.
     * @param position The position the Entity is trying to move to. directly from the second portion of the std::vector<gsl::Vector3D> movements_ container.
     * @return retuns true if they're colliding, false if not.
     */
    bool isColliding(size_t ID, std::vector<std::shared_ptr<MeshComponent> > meshComponents,
                     std::vector<std::shared_ptr<TransformComponent> > transformComponents,
                     gsl::Vector3D position);

    ///The actual container of movements. first entry is the specified Entity ID, the other is a Vector3d telling the direction they want to move.
    std::vector<std::pair<size_t,gsl::Vector3D>> movements_;


    void UpdateAIMovement(std::shared_ptr<TransformComponent> transformComponent,
                          std::shared_ptr<AIComponent> AIComponent,
                          size_t ID,
                          std::shared_ptr<Landscape> landscape_,
                          gsl::Vector3D playerPosition);
    void CheckAIState(std::shared_ptr<AIComponent> AIComponent, gsl::Vector3D AIPosition, gsl::Vector3D playerPosition);
    /**
     * Updates the Entity with an AI and Transform Component's movement around a B-Spline curve.
     * Runs whenever the AI has the AI_PATROL state.
     * @param transformComponent The Entity's specified Transform Component.
     * @param AIComponent The Entity's specified AI Component.
     * @param ID The Specified Entity's ID.
     * @param landscape_ The landscape that the entity may follow. used so that the AI follows the height of the landscape.
     */
    gsl::Vector3D AIPatrol(std::shared_ptr<TransformComponent> transformComponent,
                           std::shared_ptr<AIComponent> AIComponent,
                           std::shared_ptr<Landscape> landscape_);

    gsl::Vector3D AIChase(std::shared_ptr<TransformComponent> transformComponent,
                          std::shared_ptr<Landscape> landscape_,
                          gsl::Vector3D playerPosition);
    /**
     * Does linear interpolation for the specified AI Component. Changes the value locationOnSpline_ from 0 to 1.
     * @param AIComponent The Entity's specified AI Component.
     * @param transformComponent The Entity's specified Transform Component.
     * @param landscape_ The landscape that the entity may follow. used so that the AI follows the height of the landscape.
     */
    void Lerp(std::shared_ptr<AIComponent> AIComponent,
              std::shared_ptr<TransformComponent> transformComponent,
              std::shared_ptr<Landscape> landscape_);
    /**
     * Updates any cow that gets abducted's animation.
     * @param cowTransform The cow's transform Component.
     * @param playerTransform The Player's transform Component.
     * @param index the index of the caughtCows_ the cow is in. used to remove it when the animation's done.
     */
    void UpdateTrophyAnimation(std::shared_ptr<TransformComponent> cowTransform, std::shared_ptr<TransformComponent> playerTransform, size_t index);
    /**
     * Updates the Entity based on the parent's transform as well as their own.
     * @param transform the child's Transform Component.
     * @param transformParent the parent's Transform Component.
     */
    void UpdateFromParent(std::shared_ptr<TransformComponent> transform, std::shared_ptr<TransformComponent> transformParent);

    bool isBetweenOrdered( float val, float lowerBound, float upperBound )
    {
        return lowerBound <= val && val <= upperBound ;
    }
    bool overlaps( float min1, float max1, float min2, float max2 )
    {
        return isBetweenOrdered( min2, min1, max1 ) || isBetweenOrdered( min1, min2, max2 ) ;
    }
};

#endif // MOVEMENTSYSTEM_H
