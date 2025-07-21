import serial
import matplotlib.pyplot as plt
import numpy as np
import time


# シリアル通信でPATH:0,1,0,... を受信
def receive_path():
    ser = serial.Serial('COM8', 9600)
    time.sleep(2)
    ser.write(b"a")  # スタートの合図
    
    print("受信待機中...")
    while True:
        path_data = ser.readline().decode("utf-8").strip()
        print(path_data)
        directions = [int(x) for x in path_data if x.isdigit()]
        ser.close()
        print(directions)
        return directions

#1:左直角 2:右直角  5:直進

def rotate_vector(dx, dy, angle_rad):
    #ベクトル(dx, dy)を角度 angle_rad だけ回転
    cos_a = np.cos(angle_rad)
    sin_a = np.sin(angle_rad)
    new_dx = dx * cos_a - dy * sin_a
    new_dy = dx * sin_a + dy * cos_a
    return new_dx, new_dy

def draw_path(directions):
    x, y = 0, 0             # スタート座標
    dx, dy = 0, 1           # 初期向き（上）
    path_x = [x]
    path_y = [y]
    
    x += dx * 0.25
    y += dy * 0.25
    path_x.append(x)
    path_y.append(y)

    for dir in directions:
        if dir == 1:  # 左90度
            dx, dy = rotate_vector(dx, dy, np.pi / 2)
            x += dx * 0.5
            y += dy * 0.5
            path_x.append(x)
            path_y.append(y)

        elif dir == 2:  # 右90度
            dx, dy = rotate_vector(dx, dy, -np.pi / 2)
            x += dx * 0.5
            y += dy * 0.5
            path_x.append(x)
            path_y.append(y)

        elif dir == 5:  # 直進
            x += dx * 0.5
            y += dy * 0.5
            path_x.append(x)
            path_y.append(y)

    return path_x, path_y




# メインUI
def main():
    directions = receive_path()

    # --- 描画部分 ---
    x_list, y_list = draw_path(directions)

    plt.figure(figsize=(6, 6))
    plt.plot(x_list, y_list, linewidth=2, color="blue")
    plt.scatter(x_list[0], y_list[0], color="green", label="Start")  # スタート
    plt.scatter(x_list[-1], y_list[-1], color="red", label="Goal")   # ゴール
    plt.gca().set_aspect('equal', adjustable='box') 
    plt.axis("off")
    plt.grid(True)
    plt.legend()
    plt.title("")
    plt.show()
    
    return
    


if __name__ == "__main__":
    main()
