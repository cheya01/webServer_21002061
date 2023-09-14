<!DOCTYPE html>
<html>

<head>
    <title>Sum Result</title>
    </style>
</head>

<body style="background-color: #559451">
    <h1 style="font-size: 50px;">Sum Result</h1>
    <?php
    if (isset($_POST['num1']) && isset($_POST['num2'])) {
        $num1 = $_POST['num1'];
        $num2 = $_POST['num2'];
        $sum = $num1 + $num2;
        echo " <p style='font-size: 50px;'>The sum of $num1 and $num2 is: $sum";
    } else {
        echo "Please enter valid numbers.";
    }
    ?>
</body>

</html>