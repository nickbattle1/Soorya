#include "UIManager.h"

#include "Image/ImageDisplay.h"
#include "Widgets/IdleScreen.h"
#include "Widgets/NumbersPanel.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace ColorValues
{
    ImColor lumonBlue = ImColor(157,227,235,255);
}

class UIManagerImpl : public UIManager
{
public:
    UIManagerImpl()
    {
        imageDisplay = createImageDisplay("./assets/");
        numbersPanel = createNumbersPanel(imageDisplay);
        idleScreen = createIdleScreen(imageDisplay);
        idleMode = true; // Enable idle mode by default
        lastActivityTime = ImGui::GetTime();
    }

    void init() final {
        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        // Initialize ImGui backends
        ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
        ImGui_ImplOpenGL3_Init("#version 130");

        numbersPanel->init();
    }

    void update() final
    {
        // Toggle settings mode with 'TAB'
        if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
            settingsMode = !settingsMode;
        }

        // Manual toggle idle mode with 'I' (for testing purposes)
        if (ImGui::IsKeyPressed(ImGuiKey_I)) {
            idleMode = !idleMode;
            if (!idleMode) {
                lastActivityTime = ImGui::GetTime();
                numbersPanel->triggerLoadAnimation();
            }
        }

        // Check for user activity (mouse movement or clicks)
        bool userActivity = false;
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || 
            ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
            ImGui::IsMouseClicked(ImGuiMouseButton_Middle) ||
            ImGui::GetIO().MouseDelta.x != 0.0f || 
            ImGui::GetIO().MouseDelta.y != 0.0f) {
            userActivity = true;
        }

        if (userActivity) {
            lastActivityTime = ImGui::GetTime();
            if (idleMode) {
                idleMode = false;
                numbersPanel->triggerLoadAnimation();
            }
        }

        // Check for inactivity timeout (60 seconds)
        constexpr float idleTimeoutSeconds = 60.0f;
        if (!idleMode && (ImGui::GetTime() - lastActivityTime > idleTimeoutSeconds)) {
            idleMode = true;
        }

        if (idleMode) {
            idleScreen->update();
        } else {
            numbersPanel->update();
        }
    }

    void draw() final
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 viewportSize = viewport->WorkSize;
        ImVec2 viewportPos = viewport->WorkPos;

        static float settingsWidthRatio = 0.4f;

        // Draw widgets
        ImGui::SetNextWindowPos(ImVec2(viewportPos.x, viewportPos.y));
        ImGui::SetNextWindowSize(ImVec2(viewportSize.x, viewportSize.y));
        if (ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            if (idleMode) {
                idleScreen->drawIdleScreen();
            } else {
                numbersPanel->drawNumbersPanel();
            }
        }
        ImGui::End();

        if (settingsMode) {
            ImGui::SetNextWindowPos(viewportPos);
            ImGui::SetNextWindowSize(ImVec2(viewportSize.x * settingsWidthRatio, viewportSize.y));
            if (ImGui::Begin("Settings")) {
                numbersPanel->drawSettings();
            }
            ImGui::End();
        }
    }

    void cleanup() final {
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

private:
    std::shared_ptr<ImageDisplay> imageDisplay;
    std::shared_ptr<NumbersPanel> numbersPanel;
    std::shared_ptr<IdleScreen> idleScreen;

    bool settingsMode = false;
    bool idleMode = false;
    float lastActivityTime = 0.0f; // Track time of last user activity
};

std::shared_ptr<UIManager> createUIManager()
{
    return std::make_shared<UIManagerImpl>();
}