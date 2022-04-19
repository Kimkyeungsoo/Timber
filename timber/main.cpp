#include <SFML/Graphics.hpp>
#include <random>
#include <sstream>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

enum class side
{
	LEFT,
	RIGHT,
	NONE
};

void UpdateBranches(side sides[], int length, mt19937& gen)
{
	for (int i = length - 1; i >= 0; i--)
	{
		sides[i] = sides[i - 1];
	}

	int rnd = gen() % 5;
	switch (rnd)
	{
	case 0:
		sides[0] = side::LEFT;
		break;
	case 1:
		sides[0] = side::RIGHT;
		break;
	default:
		sides[0] = side::NONE;
		break;
	}
}

int main()
{
	random_device rd;   // non-deterministic generator
	mt19937 gen(rd());  // to seed mersenne twister.
	//uniform_int_distribution<> dist(1, 6); // 일정 범위를 구하고 싶을때

	VideoMode vm(1920, 1080);
	RenderWindow window(vm, "Timber!", Style::Default);

	Font fontKOMIKAP;
	fontKOMIKAP.loadFromFile("fonts/KOMIKAP_.ttf");

	Text textMessage;
	textMessage.setFont(fontKOMIKAP);
	Text textScore;
	textScore.setFont(fontKOMIKAP);

	textMessage.setString("Press Enter to start!");
	textScore.setString("Score = 0");

	textMessage.setCharacterSize(75);
	textScore.setCharacterSize(100);

	textMessage.setFillColor(Color::White);
	textScore.setFillColor(Color::White);

	textScore.setPosition(20, 20);

	FloatRect textRect = textMessage.getLocalBounds();
	textMessage.setOrigin(
		textRect.left + textRect.width * 0.5f,
		textRect.top + textRect.height * 0.5f
	);

	textMessage.setPosition(1920 * 0.5f, 1080 * 0.5f);
	/****************************************************
	* 이미지
	*****************************************************/
	Texture textureBackground;
	textureBackground.loadFromFile("graphics/background.png");
	Sprite spriteBackgound;
	spriteBackgound.setTexture(textureBackground);
	spriteBackgound.setPosition(0, 0);

	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");
	Sprite spriteCloud[3];
	spriteCloud[0].setTexture(textureCloud);
	spriteCloud[0].setPosition(1920, 0);
	spriteCloud[1].setTexture(textureCloud);
	spriteCloud[1].setPosition(1920, 150);
	spriteCloud[2].setTexture(textureCloud);
	spriteCloud[2].setPosition(1920, 300);

	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(1920, 750);

	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	const int countBranches = 6;
	Sprite spriteBranches[countBranches];
	side sideBranches[countBranches];
	for (int i = 0; i < countBranches; i++)
	{
		spriteBranches[i].setTexture(textureBranch);
		spriteBranches[i].setPosition(-2000, -2000);
		spriteBranches[i].setOrigin(220, 40);

		sideBranches[i] = side::NONE;
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	// The player starts on the left
	side playerSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	// Some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	bool beeActive = false;
	float beeSpeed = 0.f;

	bool cloudActive[3] = { false, false, false };
	float cloudSpeed[3] = { 0.f, 0.f, 0.f };
	/****************************************************
	* 사운드
	*****************************************************/
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound oot;
	oot.setBuffer(ootBuffer);

	/****************************************************
	* 타이머
	*****************************************************/
	RectangleShape timerBar;
	float timerBarWidth = 400;
	float timerBarHeight = 80;
	Vector2f timerBarSize = Vector2f(timerBarWidth, timerBarHeight);
	timerBar.setSize(timerBarSize);
	Vector2f timerPos = Vector2f(1920 * 0.5f - timerBarWidth * 0.5, 980.f);
	timerBar.setPosition(timerPos);
	timerBar.setFillColor(Color::Red);

	bool isPause = true;
	int score = 100;
	const float timeMax = 6.0f;
	float timeRemaining = timeMax;
	float timerBarWidthPerSecond = timerBarWidth / timeMax;

	bool acceptInput = false;

	Clock clock;


	while (window.isOpen())
	{
		Time dt = clock.restart();

		sf::Event event;
		/****************************************************
		* ALT + F4
		*****************************************************/
		/*while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}*/

		/****************************************************
		* 입력처리
		*****************************************************/
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;
			case Event::KeyPressed:
				switch (event.key.code)
				{
				case Keyboard::Escape:
					window.close();
					break;
				case Keyboard::Return:
					isPause = false;

					score = 0;
					timeRemaining = timeMax;
					acceptInput = true;

					for (int i = 0; i < countBranches; i++)
					{
						sideBranches[i] = side::NONE;
					}

					spriteRIP.setPosition(675, 2000);
					spritePlayer.setPosition(580, 720);

					break;
				case Keyboard::Left:
					if (acceptInput && !isPause)
					{
						chop.play();

						playerSide = side::LEFT;
						++score;
						timeRemaining += (2.f / score) + 0.15f;
						if (timeRemaining > timeMax)
						{
							timeRemaining = timeMax;
						}
						spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
						spritePlayer.setPosition(580, 720);

						UpdateBranches(sideBranches, countBranches, gen);

						spriteLog.setPosition(810, 720);
						logSpeedX = 5000;
						logActive = true;

						acceptInput = false;
					}
					break;
				case Keyboard::Right:
					if (acceptInput && !isPause)
					{
						chop.play();

						playerSide = side::RIGHT;
						++score;
						timeRemaining += (2.f / score) + 0.15f;
						if (timeRemaining > timeMax)
						{
							timeRemaining = timeMax;
						}
						spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
						spritePlayer.setPosition(1200, 720);

						UpdateBranches(sideBranches, countBranches, gen);

						spriteLog.setPosition(810, 720);
						logSpeedX = -5000;
						logActive = true;

						acceptInput = false;
					}
					break;
				default:
					break;
				}
				break;
			case Event::KeyReleased:
				if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right)
				{
					acceptInput = true;

					spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
				}
				break;
			default:
				break;
			}
		}

		/****************************************************
		* 업데이트
		*****************************************************/
		if (!isPause)
		{
			//벌
			if (!beeActive)
			{
				// 벌 초기화
				beeSpeed = gen() % 200 + 200;
				float y = gen() % 500 + 500;
				beeSpeed *= -1.f;
				spriteBee.setPosition(2000, y);
				beeActive = true;
			}
			else
			{
				// 벌 이동
				float deltaX = beeSpeed * dt.asSeconds();
				Vector2f currPos = spriteBee.getPosition();
				currPos.x += deltaX;
				spriteBee.setPosition(currPos);
				// 화면 밖으로 나갔는지 테스트
				if (currPos.x < -100)
				{
					beeActive = false;
				}
			}
			// 구름
			int cloudY[3] = { 0, 150, 300 };
			for (int i = 0; i < 3; i++)
			{
				if (!cloudActive[i])
				{
					// 구름 초기화
					cloudSpeed[i] = gen() % 100 + 100;
					//cloudSpeed *= -1.f;
					spriteCloud[i].setPosition(-300, cloudY[i]);
					cloudActive[i] = true;
				}
				else
				{
					// 구름 이동
					float deltaX = cloudSpeed[i] * dt.asSeconds();
					Vector2f currPos;
					currPos = spriteCloud[i].getPosition();
					currPos.x += deltaX;
					spriteCloud[i].setPosition(currPos);
					// 화면 밖으로 나갔는지 테스트
					if (currPos.x > 2100)
					{
						cloudActive[i] = false;
					}
				}
			}

			stringstream ss;
			ss << "Score = " << score;
			textScore.setString(ss.str());

			timeRemaining -= dt.asSeconds();
			timerBarSize.x = timeRemaining * timerBarWidthPerSecond;
			timerBar.setSize(timerBarSize);

			if (timeRemaining < 0.f)
			{
				oot.play();

				timerBarSize.x = 0.f;
				timerBar.setSize(timerBarSize);

				isPause = true;

				textMessage.setString("Out of time!!");
				FloatRect textRect = textMessage.getLocalBounds();
				textMessage.setOrigin(
					textRect.left + textRect.width * 0.5f,
					textRect.top + textRect.height * 0.5f
				);
			}

			for (int i = 0; i < countBranches; i++)
			{
				float height = 150 * i;

				switch (sideBranches[i])
				{
				case side::LEFT:
					spriteBranches[i].setPosition(610, height);
					spriteBranches[i].setRotation(180);
					break;
				case side::RIGHT:
					spriteBranches[i].setPosition(1330, height);
					spriteBranches[i].setRotation(0);
					break;
				default:
					spriteBranches[i].setPosition(-2000, -2000);
					break;
				}
			}

			if (logActive)
			{
				Vector2f logPos = spriteLog.getPosition();
				logPos.x += logSpeedX * dt.asSeconds();
				logPos.y += logSpeedY * dt.asSeconds();
				spriteLog.setPosition(logPos);

				if (logPos.x < -100 || logPos.x > 2000)
				{
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			if (sideBranches[countBranches - 1] == playerSide)
			{
				isPause = true;
				acceptInput = false;

				spritePlayer.setPosition(2000, 660);
				spriteRIP.setPosition(525, 760);

				textMessage.setString("SQUISHED!!");
				FloatRect textRect = textMessage.getLocalBounds();
				textMessage.setOrigin(
					textRect.left + textRect.width * 0.5f,
					textRect.top + textRect.height * 0.5f
				);

				death.play();
			}
		}


		/****************************************************
		* 드로우
		*****************************************************/
		// world
		window.clear();
		window.draw(spriteBackgound);
		for (int i = 0; i < 3; i++)
		{
			window.draw(spriteCloud[i]);
		}
		window.draw(spriteTree);
		for (int i = 0; i < countBranches; i++)
		{
			window.draw(spriteBranches[i]);
		}
		window.draw(spriteBee);
		window.draw(spritePlayer);
		window.draw(spriteLog);
		window.draw(spriteAxe);
		window.draw(spriteRIP);
		// UI
		window.draw(textScore);
		if (isPause)
		{
			window.draw(textMessage);
		}
		window.draw(timerBar);

		window.display();
	}

	return 0;
}