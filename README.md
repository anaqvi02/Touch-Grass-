# Pushup Counter with HCSR04 Sensor and 1602A LCD Screen

## Overview

This project is a simple, yet effective system that tracks the number of pushups you complete using an **HCSR04 Ultrasonic Sensor** and displays the count on a **1602A LCD Screen**. It is designed to motivate users to get outdoors and stay fit while keeping track of their progress. The system counts the pushups in real time, and there's also an online leaderboard to compare your achievements with others!

### Project Motivation
The motivation behind this project is to make physical activity, specifically pushups, more interactive and engaging. By combining hardware components with a digital leaderboard, this project aims to:
- **Encourage fitness**: Pushups are a simple and effective bodyweight exercise.
- **Gamify the workout**: Users can track their pushup count and see how they stack up against others.
- **Get outside**: Engage in physical activity outdoors and enjoy the benefits of fresh air and sunlight.
  
### Features
- **Pushup Count Tracking**: Using an **HCSR04 Ultrasonic Sensor**, the system tracks the movement of your chest to count each pushup.
- **1602A LCD Display**: Shows real-time pushup count and other helpful information like time, rank, and motivational messages.
- **Leaderboard**: Track your progress by connecting your device to a web-based leaderboard where you can see how many pushups you've done compared to others.
- **Motivational Prompts**: The LCD screen displays motivational messages to keep you going as you perform pushups.

---

## Hardware Required

- **Arduino** (or any compatible microcontroller)
- **HCSR04 Ultrasonic Sensor**: Used to measure the distance between the sensor and your chest to count each pushup.
- **1602A LCD Screen**: Displays the pushup count, leaderboard info, and motivational messages.
- **Jumper Wires**: For connecting all components together.
- **Breadboard** (optional for prototyping)
- **Power Source**: Typically a 9V battery or a USB power supply for the Arduino.

---

## Software

### Dependencies
- **Arduino IDE**: For writing and uploading the code to the Arduino.
- **Libraries**:
  - `LiquidCrystal` for controlling the LCD.
  - `Ultrasonic` for reading values from the HCSR04 sensor.

### Code Structure
1. **Pushup Counting Algorithm**: The sensor detects when your chest moves up and down (during a pushup) based on distance measurements.
2. **LCD Display**: The 1602A LCD is updated to show:
   - Real-time pushup count
   - Rank or position on the leaderboard
   - Encouraging messages like "Great job!" or "Keep going!"
3. **Leaderboard**: The project integrates with a web-based leaderboard that tracks the most pushups performed and allows users to compete.

---

## Setup and Installation

1. **Wiring the Components**:
   - Connect the **HCSR04 Ultrasonic Sensor** to your Arduino.
     - `VCC` to 5V
     - `GND` to Ground
     - `Trig` to Arduino Pin 9
     - `Echo` to Arduino Pin 10
   - Connect the **1602A LCD** to your Arduino.
     - `VCC` to 5V
     - `GND` to Ground
     - `SDA` to A4 (on most Arduino models)
     - `SCL` to A5 (on most Arduino models)

2. **Upload the Code**:
   - Open the Arduino IDE and write the code using the `LiquidCrystal` and `Ultrasonic` libraries.
   - Upload the code to your Arduino via USB.

3. **Run the Program**:
   - Once the code is uploaded, the LCD will display the number of pushups you have completed.
   - Pushup counting is triggered when the sensor detects your chest moving closer and farther from the sensor.
   - The leaderboard will update in real-time, allowing you to track your rank against others.

---

## Web Leaderboard Integration

The system integrates with a web-based leaderboard, allowing users to see how they compare with others based on their pushup counts. To access the leaderboard, visit [Your Website Here].

The leaderboard features:
- **Real-time Pushup Counts**: Display of top pushup performers.
- **User Profiles**: Show your name, total pushups, and rank.
- **Motivational Badges**: Earn badges for reaching milestones like 100 pushups, 500 pushups, etc.
  
To integrate with the website, you can send your pushup data to a server using HTTP requests or via Bluetooth if the system has wireless capability.

---

## Future Improvements
- **Mobile App Integration**: Create an app that connects with the system to view your progress on the go.
- **Workout Reminders**: Send reminders to help users stay consistent with their pushup goals.
- **Fitness Challenges**: Add challenges like daily pushup goals or competitions with friends.

---

## Contribution

If you’d like to contribute to the project, feel free to fork the repository, open pull requests, or submit issues for bugs and improvements. Your contributions are always welcome!

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

---

## Showcase
https://youtu.be/75iNrKu316g
![IMG_2184](https://github.com/user-attachments/assets/ee580e9b-45b5-44a2-bd75-62605ff3fd35)
![IMG_2185](https://github.com/user-attachments/assets/addffa55-a882-41fa-a038-6800abe00d67)
![IMG_2187](https://github.com/user-attachments/assets/1850d808-062f-4908-8bf2-eafde496d9b0)
<img width="1512" alt="Screenshot 2025-04-05 at 5 40 13 PM" src="https://github.com/user-attachments/assets/acf91a45-47ff-4cc0-9eb3-5da620513c77" />


## Acknowledgements

- Thanks to all the open-source communities for providing resources and libraries that helped build this project.
