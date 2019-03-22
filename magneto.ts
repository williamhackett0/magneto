/**
 * Provides access to all the banana functionality.
 */
// Icon unicode characters can be found at: http://fontawesome.io/icons/
//% color=190 weight=100 icon="\uf1ec" block="Magneto Blocks" advanced=false
namespace magneto {
     enum Direction {
        Left = 0,
        Right = 1,
        Up = = 2,
        Down = 3
    }
    
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
    
    /**
     * Device detected
     */
    //% weight=50
    //% blockId=magneto_device_detected_event block="Micro:bit Detected" blockGap=8
    //% shim=magneto::onDeviceDetected
    export function onDeviceDetected(body: () => void): void{
        return;   
    }

    /**
     * Device detected in specific direction
     */
    //% weight=50
    //% blockId=magneto_device_detected_direction_event block="Micro:bit Detected To My %direction" blockGap=8
    //% shim=magneto::onDeviceInDirection
    export function onDeviceDetectedDirection(direction : Direction, body: () => void): void{
        return;   
    }
    
    /**
     * Device detected with name detected
     */
    //% weight=50
    //% blockId=magneto_device_detected_name_event block="Micro:bit %name Detected" blockGap=8
    //% shim=magneto::onDeviceDetectedName
    export function onDeviceDetectedName(name : string, body: () => void): void{
        return;   
    }
    
    /**
     * Device detected with name in a specific direction
     */
    //% weight=50
    //% blockId=magneto_device_detected_name_direction_event block="Micro:bit %name Detected To My %direction" blockGap=8
    //% shim=magneto::onDeviceDetectedNameDirection
    export function onDeviceDetectedNameDirection(name : string, direction : Direction, body: () => void): void{
        return;   
    }
}
