#pragma once

#include "haven.cpp"
struct GameState
{
    // Torvid
    float torvidFrame;

    Material_defaultlit* torvidMat;

    // Barn stuff
    Material_defaultlit* barnWallMat;
    Material_defaultlit* barnWallCleanMat;
    Material_defaultlit* barnCeilingMat;
    Material_defaultlit* barnTilesMat;
    Material_defaultlit* StrawPileMat;

    Material_skydomeShader* skydomeMat;

    LightBaker* lightBaker;
    Hand* leftHand;
    Hand* rightHand;

    Player* player;

    Texture* cubemap;
    Texture* lightmap;

    StaticMesh* tonk;
};

namespace Gryphkiss
{
    void Start()
    {
        haven->gameState = ArenaPushStruct(&haven->arenaEngineState, GameState, "GameState");
        GameState* gameState = (GameState*)haven->gameState;

        // Load Torvid
        CreateSceneMaterial(gameState->torvidMat);
        gameState->torvidMat->texM1 = assets->TorvidM1;
        gameState->torvidMat->texM2 = assets->TorvidM2;
        gameState->torvidMat->texM3 = assets->TorvidM3;
        gameState->torvidMat->BackFaceCulling = true;
        gameState->torvidMat->metallicOffset = -1.0;
        
        // Create materials and load textures
        CreateSceneMaterial(gameState->barnWallMat);
        gameState->barnWallMat->texM1 = assets->BarnWallM1;
        gameState->barnWallMat->texM2 = assets->BarnWallM2;
        gameState->barnWallMat->texM3 = assets->baseM3;

        CreateSceneMaterial(gameState->barnWallCleanMat);
        gameState->barnWallCleanMat->texM1 = assets->BarnWallCleanM1;
        gameState->barnWallCleanMat->texM2 = assets->BarnWallCleanM2;
        gameState->barnWallCleanMat->texM3 = assets->baseM3;

        CreateSceneMaterial(gameState->barnCeilingMat);
        gameState->barnCeilingMat->texM1 = assets->BarnCeilingM1;
        gameState->barnCeilingMat->texM2 = assets->BarnCeilingM2;
        gameState->barnCeilingMat->texM3 = assets->baseM3;

        CreateSceneMaterial(gameState->barnTilesMat);
        gameState->barnTilesMat->texM1 = assets->TilesM1;
        gameState->barnTilesMat->texM2 = assets->TilesM2;
        gameState->barnTilesMat->texM3 = assets->baseM3;
        //gameState->barnTilesMat->roughnessOffset = 0.25;

        CreateSceneMaterial(gameState->StrawPileMat);
        gameState->StrawPileMat->texM1 = assets->StrawPileM1;
        gameState->StrawPileMat->texM2 = assets->StrawPileM2;
        gameState->StrawPileMat->texM3 = assets->baseM3;

        CreateMaterialGlobal(gameState->skydomeMat, assets->skydomeShader, skydomeShader);
        gameState->skydomeMat->Color = float3(1, 1, 1) * 3.0;
        gameState->skydomeMat->ColorTexture = assets->sky_venetian_crossroads;

        //Rendering::SetCubemap(0);
        Rendering::SetLightmap(assets->black, float3(0, 0, 0), float3(1, 1, 1), float3(2, 2, 2), 1.0f);

        gameState->leftHand  = Instantiate(Hand);
        gameState->rightHand = Instantiate(Hand);
        gameState->leftHand->handSide = HandSide_Left;
        gameState->rightHand->handSide = HandSide_Right;
        HandStart(gameState->leftHand);
        HandStart(gameState->rightHand);

        gameState->player = Instantiate(Player);
        PlayerStart(gameState->player);

        // add lighting stuffs
        ReflectionProbe* reflectionProbe = ReflectionProbeInstantiate(transform(float3(2.5, 5, 3), float3(5.2, 10, 6)));

        gameState->lightBaker = LightBakerInstantiate(assets->bake_GryphKiss, "bake/bake_GryphKiss.rad", float3(-1, -1, -1), float3(7, 12, 7), 25);
        
        gameState->tonk = StaticMeshInstantiate(assets->tonk, gameState->barnWallMat, transform(float3(2, 3, 0), float3(0.1, 0.2, 0.1)));

#if 1
        // left wall
        for (int x = 0; x < 5; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                if (y == 1)
                {
                    StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(float3(0, x * 2, y), 0, 0, -0.25));
                }
                else
                {
                    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(0, x * 2, y), 0, 0, -0.25));
                }
            }
        }
        // right wall
        for (int x = 0; x < 5; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, x * 2 + 2, y), 0, 0, 0.25));
            }
        }

        // floor
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 5; y++)
            {
                StaticMeshInstantiate(assets->tileFloor, gameState->barnTilesMat, transform(float3(x+ 0.5, y+0.5, 0) * 2, float3(1, 1, 1)));
            }
        }

        // front wall
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(x*2+2, 0, y+2), 0, 0, 0.0));
            }
        }
        StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(3, 0, 4), 0, 0, 0.0));
        StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, 0, 4), 0, 0, 0.0));
        StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(float3(1, 0, 0), 0, 0, 0.0));
        StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(float3(1, 0, 1), 0, 0, 0.0));

        StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, 0, 0), 0, 0, 0.0));
        StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, 0, 1), 0, 0, 0.0));

        // doors
        //StaticMeshInstantiate(assets->BarnDoor, gameState->barnWallCleanMat, transform(float3(1, 0, 2), 0, 0, 0.0, float3(-1, 1, -1)));
        //StaticMeshInstantiate(assets->BarnDoor, gameState->barnWallCleanMat, transform(float3(3, 0, 0), 0, 0, 0.0));

        // back wall
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(x * 2 , 10, y), 0, 0, 0.5));
            }
        }
        StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(1, 10, 4), 0, 0, 0.5));
        StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(3, 10, 4), 0, 0, 0.5));

        // ceiling left
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 5; y++)
            {
                StaticMeshInstantiate(assets->BarnCeiling01, gameState->barnWallCleanMat, transform(float3(x*0.704f, y*2, x * 0.704f) + float3(0, 2, 3), 0, 0.125, 0.25));
            }
        }

        // ceiling right
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 5; y++)
            {
                StaticMeshInstantiate(assets->BarnCeiling01, gameState->barnWallCleanMat, transform(float3(-x * 0.704f, y * 2, x * 0.704f) + float3(5, 0, 3), 0, -0.125, -0.25));
            }
        }
#endif

        StaticMeshInstantiate(assets->skydome, gameState->skydomeMat, transform(float3(0, 0, 0), float3(80, 80, 80)));
    }

    void Update()
    {
        GameState* gameState = (GameState*)haven->gameState;

        // Entity Update
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
    
        Transform torvidTransform = {
            float3(2.5, 8, 0),
            input->playspaceStageLeft.right,
            input->playspaceStageLeft.forward,
            input->playspaceStageLeft.up,
            { 1, 1, 1 } };
        torvidTransform = rotate(torvidTransform, torvidTransform.up, -0.4);
    
        Animation* torvidAnimation = assets->cubeAnim;
        Mesh* torvidMesh = assets->torvid2;
        // Set up bones
        for (int i = 0; i < torvidAnimation->boneCount; i++)
        {
            int frameOffset0 = ((int)gameState->torvidFrame);
            int frameOffset1 = ((int)gameState->torvidFrame + 1) % torvidAnimation->frameCount;
            Transform t0 = torvidAnimation->transforms[i + frameOffset0 * torvidAnimation->boneCount];
            Transform t1 = torvidAnimation->transforms[i + frameOffset1 * torvidAnimation->boneCount];
            Transform t = lerp(t0, t1, frac(gameState->torvidFrame));
            gameState->torvidMat->shaderBoneTransforms[i] = t;// torvidMesh->bindPose[i];
    
            // Draw bone
            if (haven->editor)
            {
                t.scale = vectorOne * 0.05;
                t = LocalToWorld(t, torvidTransform);
                Debug::DrawBox(t, 0.005);
                Bone* parentBone = torvidMesh->boneHierarchy[i].parent;
                if (parentBone)
                {
                    float3 parentPos = torvidAnimation->transforms[parentBone->index + frameOffset0 * torvidAnimation->boneCount].position;
                    parentPos = LocalToWorld(parentPos, torvidTransform);
                    Debug::DrawLine(parentPos, t.position);
                }
            }
        }
        
        // Play animation
        float animationFPS = 25.0f;
        if (gameState->torvidFrame == 0.0f)
        {
            PlaySound(assets->Torvid_Gryphon, 0.25f);
        }
        gameState->torvidFrame += animationFPS * input->deltaTime;
        if (gameState->torvidFrame > torvidAnimation->frameCount)
        {
            gameState->torvidFrame = 0.0f;
        }
        
        // Draw torvid
        Rendering::DrawMesh(gameState->torvidMat, torvidMesh, torvidTransform);


        input->vibrationAmplitudeRight = input->grabRight;

        input->handRight.scale = { 0.1f, 0.1f, 0.1f };
        input->handLeft.scale = { 0.1f, 0.1f, 0.1f };
        input->aimRight.scale = { 0.1f, 0.1f, 0.1f };
        input->aimLeft.scale = { 0.1f, 0.1f, 0.1f };

        float3 center = float3(0, 0, 0);
        Transform monkeyRotation = { center +
            float3(-1, 0, 0),
            input->playspaceStageLeft.right,
            input->playspaceStageLeft.forward,
            input->playspaceStageLeft.up,
            { 1, 1, 1 } };
        monkeyRotation = rotate(monkeyRotation, 0, 0, 0.0f);


        // Simulation plane
        CreateMaterialLocal(waterPlane, assets->unlit, unlit);
        waterPlane->ColorTexture = haven->waterRipplesCurrent;
        waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
        waterPlane->mesh = assets->ui_quad;
        waterPlane->BackFaceCulling = true;
        Rendering::DrawMesh(waterPlane, assets->ui_quad, transform(float3(0, 0, 0.1)));

        haven->hAlign = HAlign_left;
        haven->vAlign = VAlign_down;

        char* my_str = R"(



    This is a demo program running natively on Quest 2, written 
    from scratch in C/C++. Right now it's just an empty-ish scene. 
    I am slowly adding features and building it out.

    Check out #6c84daff#torvid.net#ffffff# to see my other projects. :>

    Controls: 
     - **Walk**: Left thumbstick.
     - **Rotate**: Right thumbstick.

    Key systems so far:
     - **Skeletal Animation** with custom tooling for blender.
     - **Baked lighting** with volumetric lightmaps and reflection probes.
     - **Text Rendering** with distance fields.
     - **Hotreloading** game code, shaders and assets.
     - Scene with #ff0000ff#X-right#ffffffff#, #00ff00ff#Y-forward#ffffffff# and #0000ffff#Z-up#ffffffff# (as god intended).
    )";

       // -**OpenXR * *for VR input
       //     - **OpenGL * *for Graphics
       //     - **OpenSL | ES * *for sound on Quest, ** DirectSound** for sound on windows.
       //     - Compiled with clang / LLVM.
        //
        //Key APIs, Librariesand tools used :
        //-OpenXR from VR input.
        //    - OpenGL - ES for graphics.
        //    - OpenSL - ES for sound.
        //    - ASTC texture compression.
        //    - "Atkinson Hyperlegible" Font.

        Debug::DrawFont(my_str, transform(center + float3(5 - 0.1, 4, 2.5), 0, 0, -0.25), 0.8, 8.0f, HAlign_right, VAlign_down);
        //Debug::DrawBox(transform(gameState->tonk->transform.position + float3(0, -2, 2.8) * 0.5, float3(6.2, 2.6, 2.8) * 0.5));
        //gameState->tonk->transform.scale = float3(1,1,1) * 0.01;
        //gameState->tonk->transform = LookRotation(gameState->tonk->transform, float3(-1, 0, 0), float3(0, 0, 1));
        //Debug::DrawBox(transform(gameState->tonk->transform.position, float3(1, 1, 1) * 0.5));
    }
}
