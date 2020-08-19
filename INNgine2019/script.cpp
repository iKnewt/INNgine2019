#include "script.h"

Script::Script()
{
    //Make variable to the path
    //We also use this in the engine.evaluate() call later
    QString fileName = gsl::scriptFilePath + "TrophyGenerationScript.js";
    //Make a QFile for it
    QFile scriptFile(fileName);

    //Try to open file and give error if something is wrong
    if (!scriptFile.open(QIODevice::ReadOnly))
        qDebug() << "Error - NO FILE HERE: " << fileName;

    //reads the file
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    //close the file, because we don't need it anymore
    scriptFile.close();

    //Loads the whole script into script engine:
    //The important part! fileName is used to report bugs in the file
    engine_.evaluate(contents, fileName);

}

std::vector<gsl::Vector3D> Script::GetSelfPlacedTrophiesXZPositions()
{
    std::vector<gsl::Vector3D> points;

    QJSValue JSpoints = engine_.evaluate("selfPlacedTrophiesXYPositions");

    const uint length = JSpoints.property("length").toUInt();
    for (uint i = 0; i < length; ++i) {

        QJSValue JSpoint = JSpoints.property(i);

        float vec[3];
        for (uint j = 0; j < 2; ++j)
        {
            vec[j] = JSpoints.property(i).property(j).toUInt();
        }
        points.push_back({vec[0], 0, vec[1]});
        qDebug() << "SET: " << points.back();
    }

    return points;
}

gsl::Vector3D Script::GetRandomXZPosition()
{
    int minX = engine_.evaluate("minX").toInt();
    int maxX = engine_.evaluate("maxX").toInt();
    int minZ = engine_.evaluate("minY").toInt();
    int maxZ = engine_.evaluate("maxY").toInt();

    static bool first = true;
    if (first)
    {
        srand( static_cast<uint>(time(nullptr))); //seeding for the first time only!
        first = false;
    }

    return {static_cast<GLfloat>(gsl::RandomNumber(minX, maxX)), 0, static_cast<GLfloat>(gsl::RandomNumber(minZ, maxZ))};
}

std::vector<gsl::Vector3D> Script::GetRandomTrophiesXZPositions()
{
    std::vector<gsl::Vector3D> points;

    int numberOfRandomTrophies = engine_.evaluate("numberOfRandomTrophies").toInt();

    for (int i = 0; i < numberOfRandomTrophies; i++)
    {
        points.push_back(GetRandomXZPosition());
        qDebug() << "RANDOM: " << points.back();
    }

    return points;
}

std::vector<gsl::Vector3D> Script::GetTrophiesXZPositions()
{
    std::vector<gsl::Vector3D> points = GetSelfPlacedTrophiesXZPositions();
    std::vector<gsl::Vector3D> randPoints = GetRandomTrophiesXZPositions();

    points.insert(points.end(), randPoints.begin(), randPoints.end());

    return points;
}

QString Script::GetMeshName()
{
    return engine_.evaluate("meshName").toString();
}

QString Script::GetMaterialName()
{
    return engine_.evaluate("materialName").toString();
}

QString Script::GetSoundName()
{
    return engine_.evaluate("soundName").toString();
}


bool Script::GetDeleteOldTrophies()
{
    return engine_.evaluate("deleteOldTrophies").toBool();
}
