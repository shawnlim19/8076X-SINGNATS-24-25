// #include "8076Pursuit/odometry.hpp"
#include "main.h"
#include <string>
#include "liblvgl/lvgl.h"
// Function to reset odometry
void resetOdom(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *screen = lv_obj_get_screen(btn);
    if (screen == lv_scr_act()) {
        chassis.calibrate(true);
    }
}

static const char *autonMap[] = {"Goal Side Safe", "Goal Side Elims", "Ring Side Safe", "\n", "Ring Side Rush", "AWP", "Skills", ""};
static const char *allianceMap[] = {"Red", "Blue", ""};

// Callback function for the selector
void switchAuton(lv_event_t *e) {
    lv_obj_t *btnmatrix = lv_event_get_target(e);
    uint16_t id = lv_btnmatrix_get_selected_btn(btnmatrix);

    // Update autonnum based on the position in autonMap
    autonnum = id + 1;

}
void switchAlliance(lv_event_t *e) {
    lv_obj_t *btnmatrix = lv_event_get_target(e);
    uint16_t id = lv_btnmatrix_get_selected_btn(btnmatrix);

    // Update autonnum based on the position in autonMap
    if (id == 0) {
        alliance = 1;
    } else if (id == 1) {
        alliance = -1;
    }
}


void displayControl() {
    //declaring the tabs on top of the screen
    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Odometry");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Selector");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Alliance");
    // odom
    lv_obj_t *odomLabel = lv_label_create(tab1);
    lv_obj_align(odomLabel, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    
    lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
    std::string odomText = "X: " + std::to_string(chassis.getPose().x) + "\n" + "Y: " + std::to_string(chassis.getPose().y) + "\n" + "Bearing: " + std::to_string(chassis.getPose().theta);
    lv_label_set_text(odomLabel, odomText.c_str());

	lv_obj_t *odomButton = lv_btn_create(tab1);
	lv_obj_set_size(odomButton, 175, 175); // Set the size of the button
	lv_obj_align(odomButton, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_add_event_cb(odomButton, resetOdom, LV_EVENT_CLICKED, NULL); // Updated function

	// Set the odom button to be a circle
	lv_obj_set_style_radius(odomButton, LV_RADIUS_CIRCLE, 0); // Set the radius to make it a circle
	lv_obj_set_style_border_width(odomButton, 2, 0); // Optional: Set border width
	//lv_obj_set_style_border_color(odomButton, lv_color_black(), 0); // Optional: Set border color
	//lv_obj_set_style_bg_color(odomButton, lv_color_blue(), 0); // Optional: Set background color

	lv_obj_t *odomButtonLabel = lv_label_create(odomButton);
	lv_label_set_text(odomButtonLabel, "Calibrate");
	lv_obj_center(odomButtonLabel); // Center the label within the button

    // selector
    lv_obj_t *autonMatrix = lv_btnmatrix_create(tab2);
    lv_btnmatrix_set_map(autonMatrix, autonMap);
    lv_btnmatrix_set_btn_ctrl_all(autonMatrix, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_set_size(autonMatrix, 450, 160);
    lv_obj_align(autonMatrix, LV_ALIGN_CENTER, 0, 0);
    lv_btnmatrix_set_one_checked(autonMatrix, true);
    lv_obj_add_event_cb(autonMatrix, switchAuton, LV_EVENT_VALUE_CHANGED, NULL); // Add event callback

    // Set the initial state based on autonnum
    if (autonnum > 0 && autonnum <= 4) {
        lv_btnmatrix_set_btn_ctrl(autonMatrix, autonnum - 1, LV_BTNMATRIX_CTRL_CHECKED);
    }

    lv_obj_t *allianceMatrix = lv_btnmatrix_create(tab3);
    lv_btnmatrix_set_map(allianceMatrix, allianceMap);
    lv_btnmatrix_set_btn_ctrl_all(allianceMatrix, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_set_size(allianceMatrix, 450, 160);
    lv_obj_align(allianceMatrix, LV_ALIGN_CENTER, 0, 0);
    lv_btnmatrix_set_one_checked(allianceMatrix, true);
    lv_obj_add_event_cb(allianceMatrix, switchAlliance, LV_EVENT_VALUE_CHANGED, NULL); // Add event callback

    // Set the initial state based on autonnum
    lv_btnmatrix_set_btn_ctrl(allianceMatrix, (alliance == 1) ? 0 : 1, LV_BTNMATRIX_CTRL_CHECKED);

    // loop for updating text
    while (true) {
        std::string autonName = (autonnum <= 3) ? autonMap[autonnum - 1] : autonMap[autonnum];
        std::string allianceName = allianceMap[(alliance == 1) ? 0 : 1];
        // std::cout << autonnum <<autonName<<std::endl;
        lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
        std::string odomText = "X: " + std::to_string(chassis.getPose().x) + "\n" + "Y: " + std::to_string(chassis.getPose().y) + "\n" + "Bearing: " + std::to_string(chassis.getPose().theta) + "\n" + "stage:" + std::to_string(stage) + "\n" + "sector: " + std::to_string(sector) + "\n" + "rotation:" + std::to_string(inertial.get_roll());
        lv_label_set_text(odomLabel, odomText.c_str());
        std::cout << allianceName << std::endl;

        pros::delay(50);
    }
}
