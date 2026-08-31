import scala.io.StdIn
object Calculator{
  def main(args: Array[String]): Unit ={
    print("Enter first number: ")
    val num1 = StdIn.readDouble()
    print("Enter operator (+, -, *, /): ")
    val op = StdIn.readChar()
    print("Enter second number: ")
    val num2 = StdIn.readDouble()

    val result = op match{
      case '+' => num1 + num2
      case '-' => num1 - num2
      case '*' => num1 * num2
      case '/' =>
        if(num2 != 0)
          num1 / num2
        else{
          println("Division by zero not possible.")
          return
        }
      case _ =>
        println("Invalid Operator")
        return
    }
    println("Result = " + result)
  }
}