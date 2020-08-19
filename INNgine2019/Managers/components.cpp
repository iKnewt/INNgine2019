#include "components.h"

void MeshComponent::read(const QJsonObject &json)
{
    objectType_ = {static_cast<ObjectType>(json["objectType"].toInt())};
    meshID_ = {static_cast<size_t>(json["meshID"].toInt())};
    materialID_ = {static_cast<size_t>(json["materialID"].toInt())};
    enableCollision_ = {json["enableCollision"].toBool()};
    reactsToFrustumCulling_  = {json["reactsToFrustumCulling"].toBool()};
}

void MeshComponent::write(QJsonObject &json) const
{
    json["objectType"] = objectType_;
    json["meshID"] = static_cast<int>(meshID_);
    json["materialID"] = static_cast<int>(materialID_);
    json["enableCollision"] = enableCollision_;
    json["reactsToFrustumCulling"] = reactsToFrustumCulling_;
}

void TransformComponent::read(const QJsonObject &json)
{
    orientRotationBasedOnMovement_ = {json["orientRotationBasedOnMovement"].toBool()};
    followLandscape_ = {json["followLandscape"].toBool()};
    position_relative_ = {json["position_relative"].toArray()};
    rotation_relative_ = {json["rotation_relative"].toArray()};
    scale_relative_ = {json["scale_relative"].toArray()};
}

void TransformComponent::write(QJsonObject &json) const
{
    json["orientRotationBasedOnMovement"] = orientRotationBasedOnMovement_;
    json["followLandscape"] = followLandscape_;
    json["position_relative"] = position_relative_.ToQJsonArray();
    json["rotation_relative"] = rotation_relative_.ToQJsonArray();
    json["scale_relative"] = scale_relative_.ToQJsonArray();
}

void AIComponent::read(const QJsonObject &json)
{
    speed_ = {static_cast<float>(json["speed"].toDouble())};
    distanceBeforeChase_ = {static_cast<float>(json["distanceBeforeChase"].toDouble())};
}

void AIComponent::write(QJsonObject &json) const
{
    json["speed"] = static_cast<double>(speed_);
    json["distanceBeforeChase"] = static_cast<double>(distanceBeforeChase_);
}

void LightComponent::read(const QJsonObject &json)
{
    lightType_ = {static_cast<LightType>(json["lightType"].toInt())};
    direction_ = {json["direction"].toArray()};
    useEntityTransformForwardVectorAsDirection_ = {json["useEntityTransformForwardVectorAsDirection"].toBool()};

    cutOff_ = {static_cast<float>(json["cutOff"].toDouble())};
    outerCutOff_ = {static_cast<float>(json["outerCutOff"].toDouble())};

    ambient_ = {json["ambient"].toArray()};
    diffuse_ = {json["diffuse"].toArray()};
    specular_ = {json["specular"].toArray()};

    constant_ = {static_cast<float>(json["constant"].toDouble())};
    linear_ = {static_cast<float>(json["linear"].toDouble())};
    quadratic_ = {static_cast<float>(json["quadratic"].toDouble())};
}

void LightComponent::write(QJsonObject &json) const
{
    json["lightType"] = lightType_;
    json["direction"] = direction_.ToQJsonArray();
    json["useEntityTransformForwardVectorAsDirection"] = useEntityTransformForwardVectorAsDirection_;

    json["cutOff"] = static_cast<double>(cutOff_);
    json["outerCutOff"] = static_cast<double>(outerCutOff_);

    json["ambient"] = ambient_.ToQJsonArray();
    json["diffuse"] = diffuse_.ToQJsonArray();
    json["specular"] = specular_.ToQJsonArray();

    json["constant"] = static_cast<double>(constant_);
    json["linear"] = static_cast<double>(linear_);
    json["quadratic"] = static_cast<double>(quadratic_);
}

void AudioComponent::read(const QJsonObject &json)
{
    soundID_ = {static_cast<ALuint>(json["soundID"].toInt())};
    isLooping_ = {json["isLooping"].toBool()};
    gain_ = {static_cast<float>(json["gain"].toDouble())};
    maxDistance_ = {static_cast<ALfloat>(json["maxDistance"].toDouble())};
}

void AudioComponent::write(QJsonObject &json) const
{
    json["soundID"] = static_cast<int>(soundID_);
    json["isLooping"] = isLooping_;
    json["gain"] = static_cast<double>(gain_);
    json["maxDistance"] = static_cast<double>(maxDistance_);
}
