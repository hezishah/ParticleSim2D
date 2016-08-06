#include "MainScreen.h"

#include <iostream>

#include "StringUtil.h"

MainScreen::
MainScreen(const std::string& title, SDL_Window* pwindow, int rwidth, int rheight)
  : nanogui::Screen(pwindow, Eigen::Vector2i(rwidth, rheight), title, false, false),
	enabled(false),
    color(0, 128, 255, 255),
	maxParticles(2048),
	particleSize(4),
	lifeTime(4),
	fade(true),
	rate(200),
	radius(10),
	angle(90),
	spread(60),
	minSpeed(160),
	maxSpeed(220),
	gravity(196)
{
	{
		auto& window = add<nanogui::Window>("Settings");
		window.withPosition({ 8, 8 }).withLayout<nanogui::BoxLayout>(nanogui::Orientation::Vertical, nanogui::Alignment::Middle, 8, 20);;
		{
			window.add<nanogui::Label>("The small circle has a particle emitter attached to it. "
				                       "You can click and drag it over using the left mouse button. "
				                       "Modified settings only apply to new particles.", 
				"sans").setFixedWidth(340);
		}

		// Panel
		auto& panel = window.add<Widget>();
		auto* layout = new nanogui::GridLayout(nanogui::Orientation::Horizontal, 2, nanogui::Alignment::Middle, 0, 5);
		layout->setColAlignment({ nanogui::Alignment::Maximum, nanogui::Alignment::Fill });
		layout->setSpacing(0, 10);
		panel.setLayout(layout);
		
		// FPS
		{
			panel.add<nanogui::Label>("FPS :", "sans-bold");
			textFPS = &panel.add<nanogui::TextBox>();
			textFPS->setFontSize(16);
			textFPS->setFixedSize(Eigen::Vector2i(100, 20));
			textFPS->setValue("0");
		}

		// Emission
		{
			panel.add<nanogui::Label>("Emission: ", "sans-bold");
			nanogui::Button& button = panel.add<nanogui::Button>("Start").withFlags(nanogui::Button::ToggleButton);
			button.setChangeCallback([=, &button](bool state)
				{
					button.setCaption(state ? "Stop" : "Start");
					setEnabled(state);
				});
		}
		
		// Color Overlay
		{
			panel.add<nanogui::Label>("Color :", "sans-bold");
			auto& popupBtn = panel.add<nanogui::PopupButton>("", 0);
			popupBtn.setBackgroundColor(color);
			popupBtn.setFontSize(16);
			popupBtn.setFixedSize(Eigen::Vector2i(100, 20));
			auto& popup = popupBtn.popup()->withLayout<nanogui::GroupLayout>();

			popup.setPosition(popupBtn.position());

			nanogui::ColorWheel& colorwheel = popup.add<nanogui::ColorWheel>();
			colorwheel.setColor(popupBtn.backgroundColor());

			nanogui::Button& colorBtn = popup.add<nanogui::Button>("");
			colorBtn.setFixedSize(Eigen::Vector2i(100, 25));
			nanogui::Color c = colorwheel.color();
			colorBtn.setBackgroundColor(c);

			colorwheel.setCallback([&colorBtn](const nanogui::Color &value) {
				colorBtn.setBackgroundColor(value);
			});

			colorBtn.setChangeCallback([=, &colorBtn, &popupBtn](bool pushed) {
				if (pushed) {
					auto& value = colorBtn.backgroundColor();
					popupBtn.setBackgroundColor(value);
					popupBtn.setPushed(false);
					setColor(value);
				}
			});
		}

		// ParticleCount
		{
			panel.add<nanogui::Label>("Particles: ", "sans-bold");
			textParticleCount = &panel.add<nanogui::TextBox>();
			textParticleCount->setAlignment(nanogui::TextBox::Alignment::Right);
			textParticleCount->setFontSize(16);
			textParticleCount->setFixedSize(Eigen::Vector2i(100, 20));
			textParticleCount->setValue("0");
		}

		// Max Particles
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 65535;
			const float INITIAL_VALUE = maxParticles;

			panel.add<nanogui::Label>("Max Particles: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat("^([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setMaxParticles(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				int k = int(MIN_VALUE + value * (MAX_VALUE - MIN_VALUE));
				textBox.setValue(std::format("%d", k));
				setMaxParticles(k);
			});
		}

		// Particle Size
		{
			const float MIN_VALUE = 0.1;
			const float MAX_VALUE = 10;
			const float INITIAL_VALUE = particleSize;

			panel.add<nanogui::Label>("Particle Size: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^(([0-9]|[1-9]([0-9]{0,1}))(\.[0-9]+)?)$|^100$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setParticleSize(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setParticleSize(k);
			});
		}

		// Life Time
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 10;
			const float INITIAL_VALUE = lifeTime;

			panel.add<nanogui::Label>("Life Time: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^([0-9](\.[0-9]+)?)$|^10$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue( (INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) );
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setLifeTime(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setLifeTime(k);
			});
		}

		// Fade
		{
			auto& checkbox = panel.add<nanogui::Label>("Fade: ", "sans-bold");
			panel.add<nanogui::CheckBox>("", [=](bool state)
			{
				setFade(state);
			})
			.withChecked(fade)
			.withFontSize(16);
		}

		// Rate
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 1000;
			const float INITIAL_VALUE = rate;

			panel.add<nanogui::Label>("Rate: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^(([0-9]|[1-9]([0-9]{0,2}))(\.[0-9]+)?)$|^1000$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setRate(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setRate(k);
			});
		}

		// Radius
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 1000;
			const float INITIAL_VALUE = radius;

			panel.add<nanogui::Label>("Radius: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^(([0-9]|[1-9]([0-9]{0,2}))(\.[0-9]+)?)$|^1000$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setRadius(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setRadius(k);
			});
		}

		// Angle
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 360;
			const float INITIAL_VALUE = angle;

			panel.add<nanogui::Label>("Angle: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^(([0-9]|[1-9]([0-9]{0,2}))(\.[0-9]+)?)$|^1000$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setAngle(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setAngle(k);
			});
		}

		// Spread
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 360;
			const float INITIAL_VALUE = spread;

			panel.add<nanogui::Label>("Spread: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^(([0-9]|[1-9]([0-9]{0,2}))(\.[0-9]+)?)$|^1000$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setSpread(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setSpread(k);
			});
		}

		// Min Speed
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 1000;
			const float INITIAL_VALUE = minSpeed;

			panel.add<nanogui::Label>("Min Initial Speed: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^(([0-9]|[1-9]([0-9]{0,2}))(\.[0-9]+)?)$|^1000$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setMinSpeed(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setMinSpeed(k);
			});
		}

		// Max Speed
		{
			const float MIN_VALUE = 0;
			const float MAX_VALUE = 1000;
			const float INITIAL_VALUE = maxSpeed;

			panel.add<nanogui::Label>("Max Initial Speed: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^(([0-9]|[1-9]([0-9]{0,2}))(\.[0-9]+)?)$|^1000$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setMaxSpeed(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setMaxSpeed(k);
			});
		}

		// Gravity
		{
			const float MIN_VALUE = -1000;
			const float MAX_VALUE = 1000;
			const float INITIAL_VALUE = gravity;

			panel.add<nanogui::Label>("Gravity: ", "sans-bold");
			auto& area = panel.add<Widget>().withLayout<nanogui::BoxLayout>(nanogui::Orientation::Horizontal, nanogui::Alignment::Maximum, 0, 16);
			auto& textBox = area.add<nanogui::TextBox>(std::format("%g", INITIAL_VALUE));
			textBox.setAlignment(nanogui::TextBox::Alignment::Right);
			textBox.setEditable(true);
			textBox.setFontSize(16);
			textBox.setFixedWidth(100);
			textBox.setDefaultValue("0");
			textBox.setFormat(R"(^0$|^([-]?([0-9]|[1-9]([0-9]{0,2}))(\.[0-9]+)?)$|^1000$)");

			auto& slider = area.add<nanogui::Slider>();
			slider.setValue((INITIAL_VALUE - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
			slider.setFixedSize(Eigen::Vector2i(100, 16));

			textBox.setCallback([=, &slider](const std::string& s)
			{
				try
				{
					float value = s.empty() ? 0 : std::stod(s);
					slider.setValue((value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE));
					setGravity(value);
					return true;
				}
				catch (std::exception&)
				{
				}

				return false;
			});

			slider.setCallback([=, &textBox](float value)
			{
				float k = MIN_VALUE + value * (MAX_VALUE - MIN_VALUE);
				textBox.setValue(std::format("%g", k));
				setGravity(k);
			});
		}
	}

	performLayout(mNVGContext);
	
}


MainScreen::
~MainScreen()
{
}

void 
MainScreen::
setFPS(int value)
{
	textFPS->setValue(std::format("%d", value));
}

void
MainScreen::
setParticleCount(int value)
{
	textParticleCount->setValue(std::format("%d", value));
}

bool 
MainScreen::
keyboardEvent(int key, int scancode, int action, int modifiers)
{
	if (Screen::keyboardEvent(key, scancode, action, modifiers))
		return true;

	return false;
}

void 
MainScreen::
draw(NVGcontext *ctx)
{
	// Update 

	// Draw the user interface
	Screen::draw(ctx);
}

void 
MainScreen::
drawContents()
{

}


bool
MainScreen::
getEnabled() { return enabled; }

nanogui::Color
MainScreen::
getColor() { return color; }

int 
MainScreen::
getMaxParticles() { return maxParticles; }

float
MainScreen::
getParticleSize() { return particleSize; }

float 
MainScreen::
getLifeTime() { return lifeTime; }

bool 
MainScreen::
getFade() { return fade; }

float 
MainScreen::
getRate() { return rate; }

float 
MainScreen::
getRadius() { return radius; }

float 
MainScreen::
getAngle() { return angle; }

float 
MainScreen::
getSpread() { return spread; }

float 
MainScreen::
getMinSpeed() { return minSpeed; }

float 
MainScreen::
getMaxSpeed() { return maxSpeed; }

float 
MainScreen::
getGravity() { return gravity; }

void
MainScreen::
setEnabled(bool value)
{
	enabled = value;
	if (enableChanged)
		enableChanged(value);
}

void 
MainScreen::
setColor(const nanogui::Color& value)
{
	color = value;
	if (colorChanged)
		colorChanged(value);
}

void 
MainScreen::
setMaxParticles(int value)
{
	maxParticles = value;
	if (maxParticlesChanged)
		maxParticlesChanged(value);
}

void
MainScreen::
setParticleSize(float value)
{
	particleSize = value;
	if (particleSizeChanged)
		particleSizeChanged(value);
}


void 
MainScreen::
setLifeTime(float value)
{
	lifeTime = value;
	if (lifeTimeChanged)
		lifeTimeChanged(value);
}

void
MainScreen::
setFade(bool value)
{
	fade = value;
	if (fadeChanged)
		fadeChanged(value);
}

void 
MainScreen::
setRate(float value)
{
	rate = value;
	if (rateChanged)
		rateChanged(value);
}

void 
MainScreen::
setRadius(float value)
{
	radius = value;
	if (radiusChanged)
		radiusChanged(value);
}

void 
MainScreen::
setAngle(float value)
{
	angle = value;
	if (angleChanged)
		angleChanged(value);
}

void 
MainScreen::
setSpread(float value)
{
	spread = value;
	if (spreadChanged)
		spreadChanged(value);
}

void 
MainScreen::
setMinSpeed(float value)
{
	minSpeed = value;
	if (minSpeedChanged)
		minSpeedChanged(value);
}

void 
MainScreen::
setMaxSpeed(float value)
{
	maxSpeed = value;
	if (maxSpeedChanged)
		maxSpeedChanged(value);
}

void 
MainScreen::
setGravity(float value)
{
	gravity = value;
	if (gravityChanged)
		gravityChanged(value);
}



