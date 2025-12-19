<?php
// PHP логіка залишається без змін
function sendCommand($cmd) {
    $socket = @fsockopen('127.0.0.1', 8080, $errno, $errstr, 2);
    if (!$socket) return "Backend Offline";
    fwrite($socket, $cmd);
    $resp = stream_get_contents($socket);
    fclose($socket);
    return $resp;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_POST['create_type'])) {
        $type = $_POST['create_type'];
        $room = str_replace('|', '', trim($_POST['room_name']));
        $name = str_replace('|', '', trim($_POST['sys_name']));
        if($room && $name) sendCommand("create|$type|$room|$name");
    } 
    elseif (isset($_POST['delete_id'])) {
        $id = $_POST['delete_id'];
        sendCommand("delete|$id|");
    }
    elseif (isset($_POST['action'])) {
        $parts = explode(':', $_POST['action']);
        sendCommand("switch|{$parts[0]}|{$parts[1]}");
    }
    elseif (isset($_POST['range_id'])) {
        $id = $_POST['range_id'];
        $val = $_POST['range_value'];
        sendCommand("set|$id|$val");
    }
}

$rawData = sendCommand("status||");
$rooms = [];

if ($rawData && $rawData !== "empty" && $rawData !== "Backend Offline") {
    $items = explode(';', $rawData);
    foreach ($items as $item) {
        if(empty($item)) continue;
        $props = explode('|', $item);
        if(count($props) >= 6) {
            $device = [
                'id' => $props[0],
                'room' => $props[1],
                'name' => $props[2],
                'type' => $props[3],
                'status' => $props[4],
                'model' => $props[5]
            ];
            $rooms[$device['room']][] = $device;
        }
    }
}
?>

<!DOCTYPE html>
<html lang="uk">
<head>
    <meta charset="UTF-8">
    <title>Smart Home - Monochrome</title>
    <link rel="stylesheet" href="css/style.css">
</head>
<body>

<div class="container">
    <h1> Керування по кімнатах</h1>

    <?php if(empty($rooms)): ?>
        <p style="text-align:center; color: var(--text-muted); padding: 40px;">Системи відсутні. Додайте першу систему нижче.</p>
    <?php else: ?>
        <?php foreach($rooms as $roomName => $devicesInRoom): ?>
            <div class="room-card">
                <div class="room-title"><?= htmlspecialchars($roomName) ?></div>
                
                <?php foreach($devicesInRoom as $dev): ?>
                    <div class="device-row">
                        <div class="device-meta">
                            <span class="sys-name">
                                <?= htmlspecialchars($dev['name']) ?>
                                <span class="status-badge"><?= htmlspecialchars($dev['status']) ?></span>
                            </span>
                            <span class="model-name"><?= htmlspecialchars($dev['model']) ?> (ID: <?= $dev['id'] ?>)</span>
                        </div>
                        
                        <div style="display:flex; align-items:center;">
                            <?php if($dev['type'] == 'switch'): ?>
                                <form method="post">
                                    <button type="submit" name="action" value="<?= $dev['id'] ?>:on" class="btn-on">ON</button>
                                    <button type="submit" name="action" value="<?= $dev['id'] ?>:off" class="btn-off">OFF</button>
                                </form>
                            
                            <?php elseif($dev['type'] == 'slider'): ?>
                                <form method="post">
                                    <input type="range" name="range_value" min="0" max="100" onchange="this.form.submit()">
                                    <input type="hidden" name="range_id" value="<?= $dev['id'] ?>">
                                    <button type="submit" name="action" value="<?= $dev['id'] ?>:off" class="btn-off">OFF</button>
                                </form>

                            <?php elseif($dev['type'] == 'thermostat'): ?>
                                <form method="post">
                                    <?php $currentTemp = filter_var($dev['status'], FILTER_SANITIZE_NUMBER_INT) ?: 22; ?>
                                    <input type="number" name="range_value" min="16" max="30" value="<?= $currentTemp ?>">
                                    <input type="hidden" name="range_id" value="<?= $dev['id'] ?>">
                                    
                                    <button type="submit" class="btn-set" title="Встановити">SET</button>
                                    <button type="submit" name="action" value="<?= $dev['id'] ?>:on" class="btn-on">COOL</button>
                                    <button type="submit" name="action" value="<?= $dev['id'] ?>:off" class="btn-off">OFF</button>
                                </form>
                            <?php endif; ?>

                            <form method="post" onsubmit="return confirm('Видалити <?= $dev['name'] ?>?');">
                                <button type="submit" name="delete_id" value="<?= $dev['id'] ?>" class="btn-del">✕</button>
                            </form>
                        </div>
                    </div>
                <?php endforeach; ?>
            </div>
        <?php endforeach; ?>
    <?php endif; ?>

    <div class="create-panel">
        <h2>➕ Додати систему</h2>
        <form method="post" class="form-row">
            <select name="create_type" required style="flex-grow:0.5;">
                <option value="light">Лампа</option>
                <option value="dimmer">Димер</option>
                <option value="fan">Вентилятор</option>
                <option value="ac">Кондиціонер</option>
            </select>
            <input type="text" name="room_name" placeholder="Кімната (напр. Вітальня)" required>
            <input type="text" name="sys_name" placeholder="Назва системи (напр. Люстра)" required>
            <button type="submit" class="btn-add">Створити</button>
        </form>
    </div>
</div>

</body>
</html>