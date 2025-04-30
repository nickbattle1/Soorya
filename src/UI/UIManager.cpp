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
        idleTimeoutEnabled = true;
        idleTimeoutSeconds = 60.0f;
        timeSinceLastActivity = 0.0f;
        // Initialize mouse positions to zero - we'll set them properly after ImGui init
        lastMousePos = ImVec2(0, 0);
        lastIdleMousePos = ImVec2(0, 0);
    }

    void init() final {
        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        // Initialize ImGui backends
        ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
        ImGui_ImplOpenGL3_Init("#version 130");

        // Now that ImGui is initialized, we can safely get the mouse position
        lastMousePos = ImGui::GetMousePos();
        lastIdleMousePos = lastMousePos;

        numbersPanel->init();
    }

    void update() final
    {
        // Toggle settings mode with 'TAB'
        if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
            settingsMode = !settingsMode;
            resetIdleTimer();
        }

        // Toggle idle mode with right click instead of 'I' key
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            idleMode = !idleMode;
            resetIdleTimer();
            if (idleMode) {
                // Reset the idle mouse position when entering idle mode
                lastIdleMousePos = ImGui::GetMousePos();
            }
        }

        // Get current mouse position
        ImVec2 mousePos = ImGui::GetMousePos();

        // Check for mouse movement to reset idle timer
        if (mousePos.x != lastMousePos.x || mousePos.y != lastMousePos.y) {
            resetIdleTimer();
            lastMousePos = mousePos;
        }

        // Update idle timer and check for timeout
        if (idleTimeoutEnabled && !idleMode) {
            timeSinceLastActivity += ImGui::GetIO().DeltaTime;
            if (timeSinceLastActivity >= idleTimeoutSeconds) {
                idleMode = true;
                // Reset the idle mouse position when entering idle mode
                lastIdleMousePos = mousePos;
            }
        }

        if (idleMode) {
            idleScreen->update();

            // Exit idle mode with 'LEFT CLICK' or ANY MOUSE MOVEMENT
            // Use the dedicated lastIdleMousePos to accurately detect movement
            bool mouseHasMoved = (mousePos.x != lastIdleMousePos.x || mousePos.y != lastIdleMousePos.y);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || mouseHasMoved) {
                idleMode = false;
                resetIdleTimer();
                numbersPanel->triggerLoadAnimation();
            }
            
            // Update the idle mouse position if we didn't exit idle mode
            if (idleMode) {
                lastIdleMousePos = mousePos;
            }
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
    void resetIdleTimer() {
        timeSinceLastActivity = 0.0f;
    }

    std::shared_ptr<ImageDisplay> imageDisplay;
    std::shared_ptr<NumbersPanel> numbersPanel;
    std::shared_ptr<IdleScreen> idleScreen;

    bool settingsMode = false;
    bool idleMode = false;
    
    // Idle timeout settings
    bool idleTimeoutEnabled;
    float idleTimeoutSeconds;
    float timeSinceLastActivity;
    ImVec2 lastMousePos;         // Used for resetting idle timer
    ImVec2 lastIdleMousePos;     // Used for detecting movement in idle mode
};

std::shared_ptr<UIManager> createUIManager()
{
    return std::make_shared<UIManagerImpl>();
}