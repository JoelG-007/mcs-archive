import scala.io.StdIn
object FibonacciSeries{
  def fibonacci(limit: Int): Unit ={

    var a = 0
    var b = 1

    print("Fibonacci Series: ")

    while(a <= limit){
      print(a + " ")
      val temp = a + b
      a = b
      b = temp
    }
  }

  def main(args: Array[String]): Unit ={
    print("Enter the limit: ")
    val limit = StdIn.readInt()

    fibonacci(limit)
  }
}