<!DOCTYPE html>
<html>

<head>
    <title>Sum Result</title>
</head>

<body>
    <h1>Sum Result</h1>
    <?php
    if (isset($_POST['num1']) && isset($_POST['num2'])) {
        $num1 = $_POST['num1'];
        $num2 = $_POST['num2'];
        $num2 = $num2 / 10;
        $sum = $num1 + $num2;
        echo "The sum of $num1 and $num2 is: $sum";
    } else {
        echo "Please enter valid numbers.";
    }
    ?>
</body>

</html>