#pragma once

#include "haven.cpp"

namespace DoorCrush
{
    struct GameState
    {
        Material_defaultlit* cornellBoxMat;
        Material_defaultlit* boxMat;
        LightBaker* lightBaker;
        Material_defaultlit* torvidMat;
        ReflectionProbe* probe0;
        ReflectionProbe* probe1;
        ReflectionProbe* probe2;
        Material_defaultlit* sphereMat;
        Material_defaultlit* sphere2Mat;
        Material_defaultlit* sphere3Mat;
        Material_defaultlit* sphere4Mat;
        Player* player;
        Hand* leftHand;
        Hand* rightHand;
        Material_skydomeShader* skydomeMat;
        StaticMesh* testMesh;
    };

    void Start()
    {
        haven->gameState = ArenaPushStruct(&haven->arenaScene, GameState, "GameState");
        GameState* gameState = (GameState*)haven->gameState;

        gameState->leftHand = Instantiate(Hand);
        gameState->rightHand = Instantiate(Hand);
        gameState->leftHand->handSide = HandSide_Left;
        gameState->rightHand->handSide = HandSide_Right;
        HandStart(gameState->leftHand);
        HandStart(gameState->rightHand);
        gameState->player = Instantiate(Player);
        PlayerStart(gameState->player);

        CreateSceneMaterial(gameState->cornellBoxMat);
        gameState->cornellBoxMat->texM1 = assets->baseM1;
        gameState->cornellBoxMat->texM2 = assets->baseM2;
        gameState->cornellBoxMat->texM3 = assets->baseM3;
        gameState->cornellBoxMat->metallicOffset = -1.0;

        CreateSceneMaterial(gameState->boxMat);
        gameState->boxMat->metallicOffset = -1.0;

        CreateSceneMaterial(gameState->sphereMat);
        gameState->sphereMat->metallicOffset = 1.0;

        CreateSceneMaterial(gameState->sphere2Mat);
        gameState->sphere2Mat->metallicOffset = 0.0;

        CreateSceneMaterial(gameState->sphere3Mat);
        gameState->sphere3Mat->texM1 = assets->TilesFloorM1;
        gameState->sphere3Mat->texM2 = assets->TilesFloorM2;
        gameState->sphere3Mat->metallicOffset = 0.0;

        CreateSceneMaterial(gameState->sphere4Mat);
        gameState->sphere4Mat->texM1 = assets->CopperSheetsM1;
        gameState->sphere4Mat->texM2 = assets->CopperSheetsM2;
        gameState->sphere4Mat->metallicOffset = 1.0;

        CreateSceneMaterial(gameState->torvidMat);
        gameState->torvidMat->texM1 = assets->TorvidM1;
        gameState->torvidMat->texM2 = assets->TorvidM2;
        gameState->torvidMat->texM3 = assets->TorvidM3;
        gameState->torvidMat->roughnessOffset = 1.5;
        gameState->torvidMat->metallicOffset = -1.0;

        CreateMaterialGlobal(gameState->skydomeMat, assets->skydomeShader, skydomeShader);
        gameState->skydomeMat->Color = float3(1, 1, 1) * 3.0;
        gameState->skydomeMat->ColorTexture = assets->sky_venetian_crossroads;
        StaticMeshInstantiate(assets->skydome, gameState->skydomeMat, transform(float3(0, 0, 0), float3(80, 80, 80)));

        {
            StaticMeshInstantiate(assets->bl_building2, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2001, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2002, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2003, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2004, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2005, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2006, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2007, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2008, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2009, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2010, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2011, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2012, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2013, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2014, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2015, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2016, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2017, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2018, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2019, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2020, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2021, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2022, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2023, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2024, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2025, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2026, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2027, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building2028, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));

            StaticMeshInstantiate(assets->bl_building1, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1001, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1002, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1003, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1004, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1005, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1006, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1007, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1008, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1009, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1010, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1011, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1012, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1013, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1014, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1015, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1016, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1017, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1018, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1019, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1020, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1021, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1022, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1023, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1024, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1025, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1026, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1027, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1029, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1030, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1031, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1032, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1033, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1034, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1035, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1036, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            gameState->testMesh = StaticMeshInstantiate(assets->bl_building1037, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1038, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1039, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_building1040, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));

            StaticMeshInstantiate(assets->bl_floor, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor002, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor003, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor004, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor005, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor006, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor007, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor008, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor009, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor010, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor011, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor012, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor013, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor014, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor015, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor016, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor017, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor018, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor019, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor020, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor021, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor022, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor023, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor024, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor025, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor026, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor027, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor028, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor029, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_floor030, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));

            StaticMeshInstantiate(assets->bl_misc, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc001, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc002, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc003, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc006, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc007, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc008, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc013, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
            StaticMeshInstantiate(assets->bl_misc014, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
        }

        gameState->lightBaker = LightBakerInstantiate(assets->bake_DoorCrush, "bake/bake_DoorCrush.rad", float3(-8, -12, -1), float3(24, 24, 14), 5);

        Transform sphere = transform();
        sphere.scale = vectorOne * 1.2;
        sphere = LookRotation(sphere, float3(0, 0, 1), float3(-1, 0, 0));

        sphere.position = float3(2, 2, 2);
        StaticMeshInstantiate(assets->sphere, gameState->sphere3Mat, sphere);

        sphere.position = float3(-1, 2, 2.1);
        StaticMeshInstantiate(assets->sphere, gameState->sphere4Mat, sphere);

        gameState->probe0 = ReflectionProbeInstantiate(transform(float3(-0.25, 0, 2), float3(7, 10, 4)));
        gameState->probe1 = ReflectionProbeInstantiate(transform(float3(7, -4, 2), float3(9, 6, 4)));
        gameState->probe2 = ReflectionProbeInstantiate(transform(float3(7, 4, 2), float3(7, 7, 4)));
    }

    void Update()
    {
        GameState* gameState = (GameState*)haven->gameState;

        HandUpdate(gameState->leftHand);
        HandUpdate(gameState->rightHand);

        PlayerUpdate(gameState->player);
        LightBakerUpdate(gameState->lightBaker);

        for (int i = 0; i < ArrayCount(haven->entities); i++)
        {
            Entity* entity = haven->entities[i];
            if (entity->type == EntityType_StaticMesh)
            {
                StaticMeshUpdate((StaticMesh*)entity, i);
            }
            if (entity->type == EntityType_ReflectionProbe)
            {
                ReflectionProbeUpdate((ReflectionProbe*)entity, i);
            }
        }


        Animation* torvidAnimation = assets->torvidTestAnim;
        for (int i = 0; i < torvidAnimation->boneCount; i++)
        {
            int frameOffset = 0;// ((int)gameState->torvidFrame)* torvidAnimation->boneCount;
            Transform t = torvidAnimation->transforms[i + frameOffset];
            gameState->torvidMat->shaderBoneTransforms[i] = t;
        }
        Rendering::DrawMesh(gameState->torvidMat, assets->torvidTest, rotate(transform(float3(0.5f, sin(input->time) * 2.5f, 0)), 0,0,0.1));

        //Drawing::DrawFont("the light ray knows where it is because it knows where it isn't", transform(float3(0, 0, 1), 0, 0, 0.25), 0.8, 8.0f, HAlign_right, VAlign_down);
    }
}
