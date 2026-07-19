#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include <vector>

using namespace std;

struct Box {
    Vector3 position;
    Vector3 size;
    Color color;
};

struct Player {
    Vector3 position;
    Vector3 velocity;
    float pitch;
    float yaw;
    float height;
    float radius;
    bool isGrounded;
};

BoundingBox GetPlayerAABB(const Player& p) {
    return {
        { p.position.x - p.radius, p.position.y, p.position.z - p.radius },
        { p.position.x + p.radius, p.position.y + p.height, p.position.z + p.radius }
    };
}

BoundingBox GetBoxAABB(const Box& b) {
    return {
        { b.position.x - b.size.x/2.0f, b.position.y - b.size.y/2.0f, b.position.z - b.size.z/2.0f },
        { b.position.x + b.size.x/2.0f, b.position.y + b.size.y/2.0f, b.position.z + b.size.z/2.0f }
    };
}

// Кастомная проверка коллизий (строго > и < вместо >= и <=)
// Это предотвращает баг, когда касание (==) считается за пересечение.
bool CheckCollisionBoxesStrict(BoundingBox box1, BoundingBox box2)
{
    return ((box1.max.x > box2.min.x) && (box1.min.x < box2.max.x) &&
            (box1.max.y > box2.min.y) && (box1.min.y < box2.max.y) &&
            (box1.max.z > box2.min.z) && (box1.min.z < box2.max.z));
}

int main(void)
{
    const int screenWidth = 1024;
    const int screenHeight = 768;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Raylib + ImGui: Custom First Person Controller");

    rlImGuiSetup(true);
    
    // ОТКЛЮЧАЕМ ВМЕШАТЕЛЬСТВО IMGUI В КУРСОР!
    // Без этого ImGui каждый кадр принудительно "освобождает" мышку из центра экрана
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    Player player = { 0 };
    player.position = { 0.0f, 0.0f, 4.0f };
    player.height = 1.8f;
    player.radius = 0.4f;
    player.yaw = PI / 2.0f; // Look towards -Z
    player.pitch = 0.0f;
    player.isGrounded = false;

    Camera3D camera = { 0 };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    std::vector<Box> boxes = {
        {{ 0, 1, -5 }, { 4, 2, 4 }, RED},
        {{ 5, 2, -5 }, { 4, 4, 4 }, BLUE},
        {{ 10, 3, -5 }, { 4, 6, 4 }, ORANGE},
        {{ -5, 0.5f, -5 }, { 4, 1, 4 }, GREEN},
        {{ 0, 4, -15 }, { 10, 8, 10 }, PURPLE}
    };

    bool isFPSMode = true;
    DisableCursor(); // Блокирует курсор в центре экрана и прячет его

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Переключение режима (Игра / Интерфейс)
        if (IsKeyPressed(KEY_TAB)) {
            isFPSMode = !isFPSMode;
            if (isFPSMode) {
                DisableCursor(); // Снова прячем и фиксируем
            } else {
                EnableCursor();  // Показываем для кликов по ImGui
            }
        }

        if (isFPSMode) {
            // Вращение камерой (сдвиг мыши работает идеально только при DisableCursor)
            Vector2 mouseDelta = GetMouseDelta();
            player.yaw -= mouseDelta.x * 0.003f;
            player.pitch -= mouseDelta.y * 0.003f;
            
            // Ограничитель, чтобы не сделать сальто назад
            player.pitch = Clamp(player.pitch, -89.0f * DEG2RAD, 89.0f * DEG2RAD);

            // Перемещение
            float speed = IsKeyDown(KEY_LEFT_SHIFT) ? 10.0f : 5.0f; // Спринт / Шаг
            
            // Вектора направления относительно угла Yaw
            Vector3 forward = { cosf(player.yaw), 0.0f, -sinf(player.yaw) };
            Vector3 right = { sinf(player.yaw), 0.0f, cosf(player.yaw) };

            Vector3 input = { 0 };
            if (IsKeyDown(KEY_W)) input = Vector3Add(input, forward);
            if (IsKeyDown(KEY_S)) input = Vector3Subtract(input, forward);
            if (IsKeyDown(KEY_A)) input = Vector3Subtract(input, right);
            if (IsKeyDown(KEY_D)) input = Vector3Add(input, right);

            if (Vector3Length(input) > 0) {
                input = Vector3Normalize(input);
                input = Vector3Scale(input, speed);
            }

            player.velocity.x = input.x;
            player.velocity.z = input.z;

            // Гравитация и прыжок
            player.velocity.y -= 25.0f * dt; 
            if (player.isGrounded && IsKeyPressed(KEY_SPACE)) {
                player.velocity.y = 10.0f; // Сила прыжка
                player.isGrounded = false;
            }
        } else {
            // Если включен курсор (ImGui режим) — останавливаем шаг, но гравитация работает
            player.velocity.x = 0;
            player.velocity.z = 0;
            player.velocity.y -= 25.0f * dt; 
        }

        // --- ФИЗИКА И КОЛЛИЗИИ ---
        // Коллизия по оси X
        player.position.x += player.velocity.x * dt;
        BoundingBox pBox = GetPlayerAABB(player);
        for (const auto& b : boxes) {
            BoundingBox bBox = GetBoxAABB(b);
            if (CheckCollisionBoxesStrict(pBox, bBox)) {
                if (player.velocity.x > 0) player.position.x = bBox.min.x - player.radius;
                else if (player.velocity.x < 0) player.position.x = bBox.max.x + player.radius;
                pBox = GetPlayerAABB(player); // Обновляем AABB после сдвига
            }
        }

        // Коллизия по оси Z
        player.position.z += player.velocity.z * dt;
        pBox = GetPlayerAABB(player);
        for (const auto& b : boxes) {
            BoundingBox bBox = GetBoxAABB(b);
            if (CheckCollisionBoxesStrict(pBox, bBox)) {
                if (player.velocity.z > 0) player.position.z = bBox.min.z - player.radius;
                else if (player.velocity.z < 0) player.position.z = bBox.max.z + player.radius;
                pBox = GetPlayerAABB(player);
            }
        }

        // Коллизия по оси Y
        player.position.y += player.velocity.y * dt;
        player.isGrounded = false;
        pBox = GetPlayerAABB(player);
        
        // Пол (земля на Y = 0)
        if (player.position.y <= 0) {
            player.position.y = 0;
            player.velocity.y = 0;
            player.isGrounded = true;
            pBox = GetPlayerAABB(player);
        }

        // Кубы по оси Y (приземление на крышу или удар головой)
        for (const auto& b : boxes) {
            BoundingBox bBox = GetBoxAABB(b);
            if (CheckCollisionBoxesStrict(pBox, bBox)) {
                if (player.velocity.y <= 0) { // Падаем на куб
                    player.position.y = bBox.max.y;
                    player.velocity.y = 0;
                    player.isGrounded = true;
                } else if (player.velocity.y > 0) { // Бьемся головой снизу
                    player.position.y = bBox.min.y - player.height;
                    player.velocity.y = 0;
                }
                pBox = GetPlayerAABB(player);
            }
        }

        // Обновление камеры к позиции игрока
        camera.position = { player.position.x, player.position.y + player.height - 0.2f, player.position.z };
        Vector3 lookDir = { cosf(player.pitch)*cosf(player.yaw), sinf(player.pitch), -cosf(player.pitch)*sinf(player.yaw) };
        camera.target = Vector3Add(camera.position, lookDir);


        // --- ОТРИСОВКА ---
        BeginDrawing();
            ClearBackground(SKYBLUE);

            BeginMode3D(camera);
                DrawPlane({0, 0, 0}, {100.0f, 100.0f}, DARKGREEN);
                DrawGrid(100, 1.0f);
                
                // Рисуем физические кубы
                for (const auto& b : boxes) {
                    DrawCube(b.position, b.size.x, b.size.y, b.size.z, b.color);
                    DrawCubeWires(b.position, b.size.x, b.size.y, b.size.z, BLACK);
                }
            EndMode3D();

            // --- IMGUI ИНТЕРФЕЙС ---
            rlImGuiBegin();

            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
            ImGui::Begin("First Person Controller HUD", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("Controls:");
            ImGui::Text("WASD - Move | SPACE - Jump | SHIFT - Sprint");
            ImGui::Text("TAB - %s", isFPSMode ? "Show Cursor (Pause)" : "Hide Cursor (Play)");
            
            ImGui::Separator();
            ImGui::Text("Player Angles:");
            ImGui::Text("Pitch: %.1f deg", player.pitch * RAD2DEG);
            ImGui::Text("Yaw:   %.1f deg", player.yaw * RAD2DEG);

            ImGui::Separator();
            ImGui::Text("Player State:");
            ImGui::Text("Speed: %.1f", Vector3Length({player.velocity.x, 0, player.velocity.z}));
            ImGui::Text("Grounded: %s", player.isGrounded ? "YES" : "NO");

            ImGui::End();

            // Прицел
            if (isFPSMode) {
                DrawCircle(screenWidth/2, screenHeight/2, 4, ColorAlpha(BLACK, 0.5f));
            }

            rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
