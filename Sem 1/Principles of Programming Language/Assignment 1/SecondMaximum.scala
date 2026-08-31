import scala.io.StdIn
object SecondMaximum{
  def main(args: Array[String]): Unit ={

    print("Enter first number: ")
    val a = StdIn.readInt()
    print("Enter second number: ")
    val b = StdIn.readInt()
    print("Enter third number: ")
    val c = StdIn.readInt()
    print("Enter fourth number: ")
    val d = StdIn.readInt()

    var max = a
    var second = Int.MinValue

    if(b > max){
      second = max
      max = b
    }else if (b > second){
      second = b
    }

    if(c > max){
      second = max
      max = c
    }else if (c > second){
      second = c
    }

    if(d > max){
      second = max
      max = d
    }else if(d > second){
      second = d
    }

    println("Second Maximum = " + second)
  }
}