/**
 * Provides access to all the banana functionality.
 */
// Icon unicode characters can be found at: http://fontawesome.io/icons/
//% color=190 weight=100 icon="\uf1ec" block="Magneto Blocks" advanced=false
namespace magneto {
    /**
     * Test function for Magneto
     */
    //% weight=50
    //% blockId=magneto_get_number block="Gets a number from magneto" blockGap=8
    //% shim=magneto::testFunction
    export function testFunction(): number
    {
        return 0;
    }
    
    /**
     * Set the Micro:bit name to be sent over detection
     */
    //% weight=50
    //% blockId=magneto_set_name block="Set Micro:bit name %name" blockGap=8
    //% shim=magneto::setMicrobitName
    export function setMicrobitName(name : string): void{
        return;   
    }
    
    /**
     * Main Detection Block. Put me in the forever loop!
     */
    //% weight=50
    //% blockId=main_function block="Main Detection Block" blockGap=8
    //% shim=magneto::main_function
    export function main_function(): void{
        return;
    }
}
