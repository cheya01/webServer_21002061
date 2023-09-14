<html>
<style>
  body {
    overflow: hidden;
    margin: 0px;
    padding: 0px;
    font-family: Arial, sans-serif;
    background: #c5bde1;
  }

  div {
    color: #ff0000;
  }

  #form {
    color: #000;
    border: 0px solid #afafaf;
    font-weight: bold;
    width: 30%;
    margin-left: 35%;
    margin-top: 120px;
    text-align: center;
    padding: 40px;
    padding-top: 20px;
    border-radius: 3px;
    box-shadow: 0px 0px 8px #777;
    background: rgba(255, 255, 255, 255);
  }

  input {
    color: #777;
    font-weight: bold;
    width: 70%;
    padding: 10px;
    margin: 10px;
    border: 1px solid #afafaf;
    border-radius: 3px;
    background: rgba(255, 255, 255, 0.5);
    outline: none;
  }

  button {
    background-color: #4caf50;
    border: none;
    color: white;
    padding: 15px 32px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
    margin: 4px 2px;
    cursor: pointer;
  }
</style>

<body>
  <div id="form">
    <h3>Adding Two Numbers</h3>
    <div class="text-success" style="text-align: center" id="error"></div>
    <form action="add.php" method="post">
      <label for="num1">Number 1:</label>
      <input type="text" name="num1" id="num1" placeholder="First Number" />
      <br />
      <label for="num2">Number 2:</label>
      <input type="text" name="num2" id="num2" placeholder="Second Number" />
      <br />
      <button name="submit" class="btn btn-primary" type="submit">
        Submit
      </button>
      <button type="reset" name="clear" class="btn btn-primary">Clear</button>
    </form>
  </div>
  <script type="text/javascript" src="jq
    uery-3.2.1.min.js"></script>
  <script>
    $(document).ready(function() {
      $("form").submit(function() {
        var num1 = $("#num1").val();
        var num2 = $("#num2").val();

        var pat = /^[0-9]+$/; //old nic

        if (num1 == "") {
          $("#error").text("Enter a value");
          $("#num1").focus();
          return false;
        }

        if (!num1.match(pat)) {
          $("#error").text("Enter a number");
          $("#num1").focus();
          return false;
        }

        if (num2 == "") {
          $("#error").text("Enter a value");
          $("#num2").focus();
          return false;
        }
        if (!num2.match(pat)) {
          $("#error").text("Enter a number");
          $("#num1").focus();
          return false;
        }
      });
    });
  </script>
  <script>
    function displayMsg(m) {
      var r = confirm("Do You want to " + m);
      if (r) {
        return true;
      } else {
        return false;
      }
    }
  </script>
</body>

</html>