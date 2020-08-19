#ifndef SCRIPT_H
#define SCRIPT_H

#include <QJSEngine>
#include <QFile>

/// Hardcoaded to read data from a spesific script file located in the Script folder.
class Script
{
public:
    /**
     * Script Constructor.
     * Loads predetermened script from file.
     */
    Script();
    /**
     * Reads all trophie positions from script file, amount based on script input.
     * @return Trophie positions based on input from script file.
     */
    std::vector<gsl::Vector3D> GetTrophiesXZPositions();
    /**
     * Gets GetDeleteOldTrophies from script file.
     * @return Whether to delete old throphies or not.
     */
    bool GetDeleteOldTrophies();
    /**
     * Gets MeshName from script file.
     * @return MeshName
     */
    QString GetMeshName();
    /**
     * Gets MaterialName from script file.
     * @return MaterialName
     */
    QString GetMaterialName();
    /**
     * Gets SoundName from script file.
     * @return SoundName
     */
    QString GetSoundName();
private:
    /// Script engine.
    QJSEngine engine_;
    /**
     * Reads selfPlacedTrophiesXYPositions from script file.
     * @return selfPlacedTrophiesXYPositions from script file.
     */
    std::vector<gsl::Vector3D> GetSelfPlacedTrophiesXZPositions();
    /**
     * Generates random XZ position within the min and max XY defined in script file.
     * @return Random XZ position within the min and max XY defined in script file.
     */
    gsl::Vector3D GetRandomXZPosition();
    /**
     * Generates random XZ positions, amount defined by numberOfRandomTrophies in script file.
     * Each position is within the min and max XY defined in script file.
     * @return Random XZ positions, amount defined by numberOfRandomTrophies in script file.
     */
    std::vector<gsl::Vector3D> GetRandomTrophiesXZPositions();
};

#endif // SCRIPT_H
