#include "raylib.h"

struct AnimData
{
  Rectangle rec;
  Vector2 pos;
  int frame;
  float updateTime;
  float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
  return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
  // update runningTime
  data.runningTime += deltaTime;
  if (data.runningTime >= data.updateTime)
  {
    data.runningTime = 0.0;
    // update animation frame
    data.rec.x = data.frame * data.rec.width;
    data.frame++;
    if (data.frame >= maxFrame)
    {
      data.frame = 0;
    }
  }
  return data;
}

int main()
{
  // window dimensions
  int windowDimensions[2];
  windowDimensions[0] = 512;
  windowDimensions[1] = 380;

  // initialize the window
  InitWindow(windowDimensions[0], windowDimensions[1], "Ruffi Run");

  // initialize the audio
  InitAudioDevice();

  // initialize music
  Sound music = LoadSound("music/8-bit-arcade-138828.wav");
  // play music
  PlaySound(music);

  // acceleration due to gravity (pixel / s / s)
  const int gravity{1'000};

  // dustBunny variables
  Texture2D dustBunny = LoadTexture("textures/12_nebula_spritesheet.png");

  // AnimData for dustBunnies
  const int sizeOfDustBunnies{20};
  AnimData dustBunnies[sizeOfDustBunnies]{};

  for (int i = 0; i < sizeOfDustBunnies; i++)
  {
    dustBunnies[i].rec.x = 0.0;
    dustBunnies[i].rec.y = 0.0;
    dustBunnies[i].rec.width = dustBunny.width / 8;
    dustBunnies[i].rec.height = dustBunny.height / 8;
    dustBunnies[i].pos.y = windowDimensions[1] - dustBunny.height / 8;
    dustBunnies[i].frame = 0;
    dustBunnies[i].runningTime = 0.0;
    dustBunnies[i].updateTime = 1.0 / 16.0;
    if (i == 0)
    {
      dustBunnies[i].pos.x = windowDimensions[0] + 300;
    }
    else
    {
      dustBunnies[i].pos.x = dustBunnies[i - 1].pos.x + GetRandomValue(250, 450);
    }
  }

  float finishLine{dustBunnies[sizeOfDustBunnies - 1].pos.x + 200};

  // dustBunny x velocity (pixels/s)
  int nebVel{-200};

  // ruffi variables
  Texture2D ruffi = LoadTexture("textures/Cat-Sprite-Sheet1.png");
  AnimData ruffiData;
  ruffiData.rec.width = ruffi.width / 8;
  ruffiData.rec.height = ruffi.height;
  ruffiData.rec.x = 0;
  ruffiData.rec.y = 0;
  ruffiData.pos.x = (windowDimensions[0] - ruffiData.rec.width) / 4;
  ruffiData.pos.y = windowDimensions[1] - ruffiData.rec.height;
  ruffiData.frame = 0;
  ruffiData.updateTime = 1.0 / 12.0;
  ruffiData.runningTime = 0.0;

  // is the rectangle in the air?
  bool isInAir{false};
  // jump velocity (pixel/s)
  const int jumpVel{-600};

  int velocity{0};

  float backgroundScale = 2.0;

  Texture2D background = LoadTexture("textures/far-buildings.png");
  float bgX{};
  Texture2D midground = LoadTexture("textures/back-buildings.png");
  float mgX{};
  Texture2D foreground = LoadTexture("textures/foreground.png");
  float fgX{};
  Texture2D title = LoadTexture("textures/ruffi-run-image.png");

  bool collision{};

  Sound catSounds[4];
  catSounds[0] = LoadSound("sound/cat-purr-meow-8327.wav");
  catSounds[1] = LoadSound("sound/cat-growl-96248.wav");
  catSounds[2] = LoadSound("sound/cat-3-43850.wav");
  catSounds[3] = LoadSound("sound/angry-cat-meow-82091.wav");

  while (!IsKeyPressed(KEY_SPACE) && !WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(WHITE);
    Vector2 titlePos{bgX, 0.0};
    DrawTextureEx(title, titlePos, 0.0, 0.5f, WHITE);

    DrawText("PRESS SPACE", windowDimensions[0] / 6, windowDimensions[1] / 4 * 3, 42, RED);

    EndDrawing();
  }

  SetTargetFPS(60);
  while (!WindowShouldClose())
  {
    // delta time (time since last fram)
    const float dT = GetFrameTime();

    // start drawing
    BeginDrawing();
    ClearBackground(WHITE);

    // scroll background
    bgX -= 20 * dT;
    if (bgX <= -background.width * 2)
    {
      bgX = 0.0;
    }

    // scroll midground
    mgX -= 40 * dT;
    if (mgX <= -midground.width * 2)
    {
      mgX = 0.0;
    }

    // scroll foreground
    fgX -= 80 * dT;
    if (fgX <= -foreground.width * 2)
    {
      fgX = 0.0;
    }

    // draw the background
    Vector2 bg1Pos{bgX, 0.0};
    DrawTextureEx(background, bg1Pos, 0.0, backgroundScale, WHITE);
    Vector2 bg2Pos{bgX + background.width * 2, 0.0};
    DrawTextureEx(background, bg2Pos, 0.0, backgroundScale, WHITE);

    // draw the midground
    Vector2 mg1Pos{mgX, 0.0};
    DrawTextureEx(midground, mg1Pos, 0.0, backgroundScale, WHITE);
    Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
    DrawTextureEx(midground, mg2Pos, 0.0, backgroundScale, WHITE);

    // draw the foreground
    Vector2 fg1Pos{fgX, 0.0};
    DrawTextureEx(foreground, fg1Pos, 0.0, backgroundScale, WHITE);
    Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
    DrawTextureEx(foreground, fg2Pos, 0.0, backgroundScale, WHITE);

    // perform ground check
    if (isOnGround(ruffiData, windowDimensions[1]))
    {
      // rectangle is on the ground
      velocity = 0;
      isInAir = false;
    }
    else
    {
      // rectangle is in the air
      // apply gravity
      velocity += gravity * dT;
      isInAir = true;
    }

    // jump check
    if (IsKeyPressed(KEY_SPACE) && !isInAir)
    {
      velocity += jumpVel;
      PlaySound(catSounds[GetRandomValue(0, 3)]);
    }

    for (int i = 0; i < sizeOfDustBunnies; i++)
    {
      // update each dustBunny position
      dustBunnies[i].pos.x += nebVel * dT;
    }

    // update finish line
    finishLine += nebVel * dT;

    // update ruffi position
    ruffiData.pos.y += velocity * dT;

    // update ruffi's animation frame
    if (!isInAir)
    {
      ruffiData = updateAnimData(ruffiData, dT, 7);
    }

    for (int i = 0; i < sizeOfDustBunnies; i++)
    {
      dustBunnies[i].runningTime += dT;
      dustBunnies[i] = updateAnimData(dustBunnies[i], dT, 7);
    }

    for (AnimData dustBunny : dustBunnies)
    {
      float pad{50};
      Rectangle nebRec{
          dustBunny.pos.x + pad,
          dustBunny.pos.y + pad,
          dustBunny.rec.width - 2 * pad,
          dustBunny.rec.height - 2 * pad};
      Rectangle ruffiRec{
          ruffiData.pos.x,
          ruffiData.pos.y,
          ruffiData.rec.width,
          ruffiData.rec.height};
      if (CheckCollisionRecs(nebRec, ruffiRec))
      {
        collision = true;
      }
    }

    if (collision)
    {
      // lose the game
      DrawText("Game Over!", windowDimensions[0] / 3, windowDimensions[1] / 2, 42, RED);
    }
    else if (ruffiData.pos.x >= finishLine)
    {
      // win the game
      DrawText("You Win!", windowDimensions[0] / 3, windowDimensions[1] / 2, 42, GREEN);
    }
    else
    {
      for (int i = 0; i < sizeOfDustBunnies; i++)
      {
        // draw each dustBunny
        DrawTextureRec(dustBunny, dustBunnies[i].rec, dustBunnies[i].pos, WHITE);
      }

      // draw ruffi
      DrawTextureRec(ruffi, ruffiData.rec, ruffiData.pos, WHITE);
    }

    // stop drawing
    EndDrawing();
  }
  UnloadTexture(ruffi);
  UnloadTexture(dustBunny);
  UnloadTexture(background);
  UnloadTexture(midground);
  UnloadTexture(foreground);
  UnloadTexture(title);
  UnloadSound(catSounds[0]);
  UnloadSound(catSounds[1]);
  UnloadSound(catSounds[2]);
  UnloadSound(catSounds[3]);
  UnloadSound(music);
  CloseAudioDevice();
  CloseWindow();
}